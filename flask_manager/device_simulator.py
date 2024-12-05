import flask

app = flask.Flask(__name__)

# Храним состояние устройства
device_state = {
    "device_name": "Generic Device",
    "device_type": "switch",
    "status": "off"  # Начальное состояние устройства
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

    device_state["status"] = new_status
    return flask.jsonify({
        "message": f"Device turned {new_status}",
        "device_name": device_state["device_name"],
        "status": device_state["status"]
    })


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
