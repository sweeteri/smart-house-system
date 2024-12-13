from flask import Flask, request, jsonify
import docker
import re
import unidecode
import os
import shutil
import logging
import ray

logging.basicConfig(level=logging.DEBUG)
app = Flask(__name__)
client = docker.from_env()

# Инициализация Ray
ray.init()

# Утилиты
def sanitize_name(name):
    sanitized = unidecode.unidecode(name)
    sanitized = re.sub(r'[^a-zA-Z0-9_-]', '_', sanitized)
    return sanitized.lower()

def select_template(device_type, device_name):
    templates = {
        "on_off_devices": "devices_templates/lighting_template.py",
        "on_off_temp_devices": "devices_templates/heating_template.py",
        "humidifier": "devices_templates/humidifier_template.py"
    }
    if device_type in ['освещение', 'безопасность', 'бытовая техника']:
        return templates['on_off_devices']
    elif device_type == "отопление" and device_name.startswith("увлажнитель"):
        return templates['humidifier']
    elif device_type == "отопление":
        return templates['on_off_temp_devices']
    return 'not found'

# Распределённые задачи Ray
@ray.remote
def simulate_device(device_name, room_name, action):
    """Эта задача получает только сериализуемые данные."""
    logging.info(f"Simulating {action} action for device {device_name} in room {room_name}")
    return {
        "device_name": device_name,
        "room_name": room_name,
        "action": action,
        "status": "success"
    }

@ray.remote
def create_room_sensor_task(room_name, sensor_type):
    """Создание сенсора в комнате в распределённой задаче."""
    try:
        sanitized_room_name = sanitize_name(room_name)
        container_name = f"sensor_{sanitized_room_name}_{sensor_type}"
        build_dir = os.path.join('/tmp', container_name)
        os.makedirs(build_dir, exist_ok=True)

        # Копируем шаблон сенсора
        template_path = f"sensors_templates/{sensor_type}_sensor_template.py"
        shutil.copy(template_path, os.path.join(build_dir, 'sensor_simulator.py'))

        # Создаём Dockerfile
        dockerfile_content = f"""
        FROM python:3.9-slim
        COPY . /app
        WORKDIR /app
        RUN pip install flask
        CMD ["python", "sensor_simulator.py"]
        """
        with open(os.path.join(build_dir, 'Dockerfile'), 'w') as dockerfile:
            dockerfile.write(dockerfile_content)

        # Создаём Docker-образ
        image, _ = client.images.build(path=build_dir, tag=f"{container_name}:latest")
        container = client.containers.create(
            image=f"{container_name}:latest",
            name=container_name,
            detach=True,
            environment={
                "ROOM_NAME": room_name,
                "SENSOR_TYPE": sensor_type
            },
            network="smarthousesystem_app-network"
        )
        container.start()
        return {"status": "success", "container_name": container_name}
    except Exception as e:
        logging.error(f"Error creating container {container_name}: {e}")
        return {"status": "error", "error": str(e)}
    finally:
        if os.path.exists(build_dir):
            shutil.rmtree(build_dir)

# Эндпоинты
@app.route('/create_image', methods=['POST'])
def create_image():
    logging.info("Received request to create an image")
    data = request.json
    device_name = data.get('device_name')
    device_type = data.get('device_group')
    room_name = data.get('room_name')

    if not device_name or not room_name:
        logging.error("Missing required parameters: device_name or room_name")
        return jsonify({"error": "Device name and room name are required"}), 400

    sanitized_device_name = sanitize_name(device_name)
    sanitized_room_name = sanitize_name(room_name)
    container_name = f"device_{sanitized_room_name}_{sanitized_device_name}"

    build_dir = os.path.join('/tmp', container_name)

    try:
        # Создание сенсоров через Ray
        if device_type == "отопление":
            ray.get(create_room_sensor_task.remote(room_name, "temperature"))
            if device_name.startswith("увлажнитель"):
                ray.get(create_room_sensor_task.remote(room_name, "humidity"))

        os.makedirs(build_dir, exist_ok=True)

        # Копируем шаблон устройства
        template_path = select_template(device_type, device_name)
        if template_path == 'not found' or not os.path.exists(template_path):
            logging.error(f"Template for device type '{device_name}{device_type}' not found")
            return jsonify({"error": f"Template for device type '{device_name}{device_type}' not found"}), 400

        shutil.copy(template_path, os.path.join(build_dir, 'device_simulator.py'))

        # Создаём Dockerfile
        dockerfile_content = f"""
        FROM python:3.9-slim
        COPY . /app
        WORKDIR /app
        RUN pip install flask
        CMD ["python", "device_simulator.py"]
        """
        with open(os.path.join(build_dir, 'Dockerfile'), 'w') as dockerfile:
            dockerfile.write(dockerfile_content)

        # Создаём Docker-образ
        image, _ = client.images.build(path=build_dir, tag=f"{container_name}:latest")
        container = client.containers.create(
            image=f"{container_name}:latest",
            name=container_name,
            detach=True,
            environment={
                "DEVICE_NAME": device_name,
                "DEVICE_TYPE": device_type,
                "ROOM": room_name
            },
            network="smart-house-system_app-network"
        )
        container.start()

        return jsonify({"message": f"Image and container for {container_name} created successfully"}), 201
    except Exception as e:
        logging.exception("An error occurred while creating the image and container")
        return jsonify({"error": str(e)}), 500
    finally:
        if os.path.exists(build_dir):
            shutil.rmtree(build_dir)

# Другие эндпоинты остаются прежними...

@app.route('/toggle_device', methods=['POST'])
def toggle_device():
    data = request.json
    device_name = data.get('device_name')
    room = data.get('room_name')
    action = data.get('action')  # "start" или "stop"

    if not device_name or not room or action not in ["start", "stop"]:
        return jsonify({'error': 'Invalid parameters'}), 400

    sanitized_name = f"device_{sanitize_name(room)}_{sanitize_name(device_name)}"

    try:
        # Используем Ray для симуляции устройства
        result = ray.get(simulate_device.remote(device_name, room, action))
        return jsonify(result), 200
    except Exception as e:
        app.logger.error(f"Unexpected error: {str(e)}")
        return jsonify({'error': str(e)}), 500

@app.route('/toggle_scenario', methods=['POST'])
def toggle_scenario():
    data = request.json
    scenario_name = data.get("scenario_name")
    devices = data.get("devices")
    action = data.get("action")

    if not scenario_name or not devices or action not in ["activate", "deactivate"]:
        return jsonify({"success": False, "message": "Invalid parameters"}), 400

    # Запуск задач в Ray
    tasks = [
        simulate_device.remote(device.split(": ")[1], device.split(": ")[0], action)
        for device in devices
    ]

    results = ray.get(tasks)  # Получение всех результатов

    all_success = all(r["status"] == "success" for r in results)
    return jsonify({
        "success": all_success,
        "message": f"Scenario {action}d {'successfully' if all_success else 'with some errors'}",
        "details": results
    })


@app.route('/device_status', methods=['GET'])
def device_status():
    device_name = request.args.get('device_name')
    room_name = request.args.get('room_name')  # Учитываем комнату

    if not device_name or not room_name:
        return jsonify({'error': 'Device name and room name are required'}), 400

    sanitized_device_name = sanitize_name(device_name)
    sanitized_room_name = sanitize_name(room_name)

    container_name = f"device_{sanitized_room_name}_{sanitized_device_name}"
    try:
        container = client.containers.get(container_name)
        return jsonify({
            "device_name": device_name,
            "room_name": room_name,
            "status": container.status
        }), 200
    except docker.errors.NotFound:
        return jsonify({'error': f'Container for {device_name} in room {room_name} not found'}), 404
    except Exception as e:
        return jsonify({'error': str(e)}), 500


if __name__ == '__main__':
    # Flask приложение с Ray
    app.run(host='0.0.0.0', port=5000)

