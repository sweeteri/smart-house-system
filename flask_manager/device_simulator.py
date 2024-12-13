import flask
import ray

# Инициализация Ray
ray.init()

app = flask.Flask(__name__)

# Храним состояние устройства
device_state = {
    "device_name": "Generic Device",
    "device_type": "switch",
    "status": "off"  # Начальное состояние устройства
}


@ray.remote
def toggle_device_task(device_name, device_type, new_status):
    """
    Задача Ray для включения/выключения устройства.
    """
    if new_status not in ["on", "off"]:
        return {"error": "Invalid status. Use 'on' or 'off'."}

    # Эмулируем обработку действия
    return {
        "device_name": device_name,
        "device_type": device_type,
        "status": new_status,
        "message": f"Device turned {new_status}"
    }


@app.route('/status', methods=['GET'])
def get_status():
    """
    Возвращает текущее состояние устройства.
    """
    return flask.jsonify({
        "device_name": device_state["device_name"],
        "device_type": device_state["device_type"],
        "status": device_state["status"]
    })


@app.route('/toggle', methods=['POST'])
def toggle_device():
    """
    Включает или выключает устройство.
    """
    new_status = flask.request.json.get("status")
    if new_status not in ["on", "off"]:
        return flask.jsonify({"error": "Invalid status. Use 'on' or 'off'."}), 400

    # Выполнение через Ray
    task_result = ray.get(toggle_device_task.remote(
        device_state["device_name"],
        device_state["device_type"],
        new_status
    ))

    # Обновление состояния устройства на основе результата
    if "error" not in task_result:
        device_state["status"] = task_result["status"]

    return flask.jsonify(task_result)


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)

