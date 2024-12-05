from flask import Flask, jsonify, request

app = Flask(__name__)

state = "off"
humidity = 50

@app.route('/status', methods=['GET'])
def status():
    return jsonify({
        "device_name": "{device_name}",
        "device_type": "{device_type}",
        "room_name": "{room_name}",
        "state": state,
        "humidity": humidity
    })

@app.route('/toggle', methods=['POST'])
def toggle():
    global state
    state = "on" if state == "off" else "off"
    return jsonify({"device_name": "{device_name}", "state": state})

@app.route('/humidity', methods=['POST'])
def set_humidity():
    global humidity
    hum = request.json.get('humidity')
    if hum < 30 or hum > 70:
        return jsonify({"error": "Humidity out of range"}), 400
    humidity = hum
    return jsonify({"device_name": "{device_name}", "humidity": humidity})

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
