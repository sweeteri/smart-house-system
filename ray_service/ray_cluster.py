import ray

if __name__ == "__main__":
    # Запуск Ray с поддержкой Dashboard
    ray.init(address="auto", dashboard_host="0.0.0.0")
    print("Ray service is running and ready to receive tasks...")
    while True:
        pass  # Ray-кластер работает в фоновом режиме
