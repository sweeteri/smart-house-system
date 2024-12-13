import threading
import random
from flask import Flask, jsonify, request

app = Flask(__name__)

@app.route('/sensor_data', methods=['GET'])
def sensor_data():
    temperature = random.randint(15, 30)  # Случайное значение температуры
    return jsonify({
        "temperature": temperature
    }), 200


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
