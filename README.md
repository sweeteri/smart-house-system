# Distributed Smart Home System  

## Overview  

The **Smart Home Automation System** is an advanced platform designed for simulating, controlling, and automating smart home devices. The system uses a combination of **simulated devices** and scenarios to provide a realistic environment for testing and managing smart home setups. All devices are virtual and operate as Docker containers managed by a Flask-based API, and the system integrates **Ray.io** to handle distributed task execution.  

### Key Features  
- **Simulated Devices**: All devices are virtual, running as Docker containers that mimic the behavior of real-world devices.  
- **Scenario Automation**: Users can create and manage scenarios to automate device interactions.  
- **Ray.io Integration**: Distributed task execution and resource management using Ray.io, integrated into the Flask Manager.  
- **Extensibility**: Add new device types, sensors, or scenarios without major changes to the core system.  

---

## Architecture  

The project consists of three main components:  
1. **Qt Client**:  
   - A desktop application built with Qt that provides an intuitive interface for users to interact with the system.  
   - Features include device management, scenario creation, and real-time monitoring.  

2. **QT Central Server**:  
   - Handles communication between the client and Flask Manager.  
   - Manages the PostgreSQL database for storing devices, scenarios, and user-defined settings.  

3. **Flask Manager with Ray.io**:  
   - Manages device simulation and interactions using Docker containers.  
   - Integrates Ray.io for distributed execution of tasks such as managing scenarios and device updates.  

---

## Installation  

### Prerequisites  
Ensure the following are installed on your system:  
- [Docker](https://www.docker.com/)  
- [Docker Compose](https://docs.docker.com/compose/)  
- [PostgreSQL](https://www.postgresql.org/)  
- [Qt Framework](https://www.qt.io/)  
- Python 3.9 or later  

### Setup Instructions  

1. **Clone the Repository**  
   ```bash  
   git clone https://github.com/sweeteri/smart-house-system  
   cd smart-house-system
   ```
2. **Create DB folder**
    ```bash
    mkdir test_db
    ```
3. ***Create .env file***
    ```bash
    USER=your username
    PASSWORD=your password
    DB_NAME=smarthouse
    DB_HOST=database
    DB_PORT=your port(e.g.5432)
    APP_PORT=your port(e.g.1234)
    ```
4. ***Run Docker compose***
    ```bash
   docker-compose up --build
    ```
5. ***Run QT Client***
    - Open the Qt project in Qt Creator and build the application.
    - Run the client to start interacting with the system.
