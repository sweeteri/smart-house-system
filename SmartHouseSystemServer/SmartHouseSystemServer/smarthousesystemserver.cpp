#include "smarthousesystemserver.h"
#include "databasemanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QSqlQuery>

SmartHouseSystemServer::SmartHouseSystemServer(QObject *parent)
    : QTcpServer(parent) {
    if (!this->listen(QHostAddress::LocalHost, 1234)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started!";
    }

    connect(this, &QTcpServer::newConnection, this, [this]() {
        QTcpSocket *socket = this->nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
            while (socket->canReadLine()) {
                QByteArray line = socket->readLine();
                QJsonParseError parseError;
                QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);
                QJsonObject request = doc.object();

                qDebug() << "----IBUSKO---- QJsonObject request = doc.object();" << request;

                if (parseError.error != QJsonParseError::NoError) {
                    return;
                }

                QString action = request["action"].toString();
                if (action == "register") {
                    processRegistrationRequest(socket, request);
                } else if (action == "login") {
                    processLoginRequest(socket, request);
                } else if (action == "loadRooms") {
                    processRoomLoadRequest(socket, request);
                } else if (action == "addRoom") {
                    processAddRoomRequest(socket, request);
                } else if (action == "loadAllDevices") {
                    processLoadDevicesRequest(socket, request);
                } else if (action == "loadScenarios") {
                    processLoadScenariosRequest(socket, request);
                } else if (action == "addScenario") {
                    processAddScenarioRequest(socket, request);
                } else if (action == "addDevice") {
                    processAddDeviceRequest(socket, request);
                } else if (action=="loadRoomDevices"){
                    processRoomDevicesRequest(socket, request);
                }

            }
        });

        connect(socket, &QTcpSocket::disconnected, this, [socket]() {
            qDebug() << "Socket disconnected from:" << socket->peerAddress().toString();
            socket->deleteLater();
        });
    });

}

void SmartHouseSystemServer::processRegistrationRequest(QTcpSocket *socket, const QJsonObject &request) {
    QString username = request["username"].toString();
    QString password = request["password"].toString();
    QString role = request["role"].toString();
    bool adminExists = DatabaseManager::instance().adminExists();
    bool checkRole = ((role=="admin")&&(adminExists))? true: false;
    if (DatabaseManager::instance().userExists(username)) {
        QJsonObject response;
        response["success"] = false;
        response["message"] = "Username already exists.";
        socket->write(QJsonDocument(response).toJson());
        socket->flush();

        return;
    }
    if (checkRole){
        QJsonObject response;
        response["success"] = false;
        response["message"] = "Admin already exist. You cannot be admin.";
        socket->write(QJsonDocument(response).toJson());
        socket->flush();

        return;
    }

    if (DatabaseManager::instance().registerUser(username, password, role)) {
        QJsonObject response;
        response["success"] = true;
        response["message"] = "User registered successfully.";
        socket->write(QJsonDocument(response).toJson());
    } else {
        QJsonObject response;
        response["success"] = false;
        response["message"] = "Registration failed.";
        socket->write(QJsonDocument(response).toJson());
    }

    socket->flush();

}

void SmartHouseSystemServer::processLoginRequest(QTcpSocket *socket, const QJsonObject &request) {
    QString username = request["username"].toString();
    QString password = request["password"].toString();

    if (DatabaseManager::instance().authenticateUser(username, password)) {
        QString userRole = DatabaseManager::instance().getUserRole(username);
        QJsonObject response;
        response["authenticated"] = true;
        response["role"] = userRole;
        response["message"] = "Login successful.";
        socket->write(QJsonDocument(response).toJson());
    } else {
        QJsonObject response;
        response["authenticated"] = false;
        response["message"] = "Invalid username or password.";
        socket->write(QJsonDocument(response).toJson());
    }

    socket->flush();

}

void SmartHouseSystemServer::processRoomLoadRequest(QTcpSocket *socket, const QJsonObject &request) {
    QJsonArray roomList = QJsonArray::fromStringList(DatabaseManager::instance().getAllRooms());
    QJsonObject response;
    response["action"] = "loadRooms";
    response["rooms"] = roomList;
    qDebug()<< "----IBUSKO---- processRoomLoadRequest " << response;
    socket->write(QJsonDocument(response).toJson());
    socket->flush();

}
void SmartHouseSystemServer::processRoomDevicesRequest(QTcpSocket *socket, const QJsonObject &request) {
    QString roomName = request["room"].toString();
    QStringList devices = DatabaseManager::instance().getDevicesForRoom(roomName);

    QJsonArray deviceArray;
    for (const QString &device : devices) {
        deviceArray.append(device);
    }

    QJsonObject response;
    response["action"] = "loadRoomDevices";
    response["room"] = roomName;
    response["roomDevices"] = deviceArray;

    socket->write(QJsonDocument(response).toJson());
    socket->flush();
}
void SmartHouseSystemServer::processAddRoomRequest(QTcpSocket *socket, const QJsonObject &request) {
    QString roomName = request["roomName"].toString();
    bool success = DatabaseManager::instance().addRoom(roomName);

    QJsonObject response;
    response["action"] = "addRoom";
    response["success"] = success;
    response["message"] = success ? "Room added successfully." : "Failed to add room.";
    socket->write(QJsonDocument(response).toJson());
    socket->flush();
}

void SmartHouseSystemServer::processLoadDevicesRequest(QTcpSocket *socket, const QJsonObject &request) {
    QMap<QString, QStringList> devices = DatabaseManager::instance().getAllDevices();
    QJsonObject response;
    response["action"] = "loadAllDevices";

    QJsonArray devicesArray;
    for (auto it = devices.begin(); it != devices.end(); ++it) {
        QJsonObject deviceObject;
        deviceObject["type"] = it.key();
        deviceObject["rooms"] = QJsonArray::fromStringList(it.value());
        devicesArray.append(deviceObject);
    }
    response["devices"] = devicesArray;
    socket->write(QJsonDocument(response).toJson());
    socket->flush();
}

void SmartHouseSystemServer::processAddDeviceRequest(QTcpSocket *socket, const QJsonObject &request) {
    QString roomName = request["roomName"].toString();
    QString deviceType = request["deviceType"].toString();

    QString generatedDeviceName; // Переменная для хранения имени устройства
    bool success = DatabaseManager::instance().addDevice(roomName, deviceType, generatedDeviceName);

    QJsonObject response;
    response["action"] = "addDevice";
    response["success"] = success;
    response["message"] = success ? "Device added successfully." : "Failed to add device.";

    if (success) {
        response["deviceName"] = generatedDeviceName; // Возвращаем имя устройства клиенту
    }

    socket->write(QJsonDocument(response).toJson());
    socket->flush();
}



void SmartHouseSystemServer::processLoadScenariosRequest(QTcpSocket *socket, const QJsonObject &request) {
    QJsonArray scenarioList = QJsonArray::fromStringList(DatabaseManager::instance().getAllScenarios());

    QJsonObject response;
    response["action"] = "loadScenarios";
    response["scenarios"] = scenarioList;
    socket->write(QJsonDocument(response).toJson());
    socket->flush();
}

void SmartHouseSystemServer::processAddScenarioRequest(QTcpSocket *socket, const QJsonObject &request) {
    QString scenarioName = request["scenarioName"].toString();
    bool success = DatabaseManager::instance().addScenario(scenarioName);

    QJsonObject response;
    response["action"] = "addScenario";
    response["success"] = success;
    response["message"] = success ? "Scenario added successfully." : "Failed to add scenario.";
    socket->write(QJsonDocument(response).toJson());
    socket->flush();
}

