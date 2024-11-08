#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");

    QString dbPath = QDir(QCoreApplication::applicationDirPath()).filePath("database.sqlite");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Database error occurred:" << db.lastError().text();
    } else {
        initializeDatabase();
    }
}


DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::openConnection() {
    if (!db.open()) {
        qDebug() << "Database connection error:" << db.lastError().text();
        return false;
    }
    qDebug() << "Database connection successful.";
    return true;
}

QSqlDatabase DatabaseManager::getDatabase() {
    return db;
}
void DatabaseManager::initializeDatabase() {
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "username TEXT UNIQUE, "
                    "password TEXT, "
                    "role TEXT)")) {
        qDebug() << "Failed to create users table:" << query.lastError().text();
    }
    if (!query.exec("CREATE TABLE IF NOT EXISTS rooms ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT UNIQUE)")) {
        qDebug() << "Failed to create rooms table:" << query.lastError().text();
    }
    if (!query.exec("CREATE TABLE IF NOT EXISTS devices ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "room_id INTEGER, "
                    "name TEXT, "
                    "FOREIGN KEY (room_id) REFERENCES rooms(id))")) {
        qDebug() << "Failed to create devices table:" << query.lastError().text();
    }
    if (!query.exec("CREATE TABLE IF NOT EXISTS scenarios ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT UNIQUE)")) {
        qDebug() << "Failed to create scenarios table:" << query.lastError().text();
    }
}
bool DatabaseManager::registerUser(const QString &username, const QString &password, const QString &role) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password, role) VALUES (:username, :password, :role)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":role", role);

    if (!query.exec()) {
        qDebug() << "Failed to register user: " << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::authenticateUser(const QString &username, const QString &password) {
    QSqlQuery query;
    query.prepare("SELECT password FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        QString storedPassword = query.value(0).toString();
        return (storedPassword == password);
    }
    return false;
}
QString DatabaseManager::getUserRole(const QString &username) {
    QSqlQuery query;
    query.prepare("SELECT role FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return "";
}
bool DatabaseManager::userExists(const QString &username) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}
bool DatabaseManager::adminExists() {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE role = 'admin'");

    if (query.exec() && query.next()) {
        int adminCount = query.value(0).toInt();
        return adminCount > 0;
    }
    return false;
}
bool DatabaseManager::addRoom(const QString &roomName) {
    QSqlQuery query;
    query.prepare("INSERT INTO rooms (name) VALUES (:name)");
    query.bindValue(":name", roomName);

    if (!query.exec()) {
        qDebug() << "Failed to add room: " << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::addDevice(const QString &roomName, const QString &deviceName) {
    QSqlQuery query;

    query.prepare("SELECT id FROM rooms WHERE name = :name");
    query.bindValue(":name", roomName);

    if (!query.exec() || !query.next()) {
        qDebug() << "Room not found: " << roomName;
        return false;
    }

    int roomId = query.value(0).toInt();

    query.prepare("INSERT INTO devices (room_id, name) VALUES (:room_id, :name)");
    query.bindValue(":room_id", roomId);
    query.bindValue(":name", deviceName);

    if (!query.exec()) {
        qDebug() << "Failed to add device: " << query.lastError().text();
        return false;
    }
    return true;
}
QStringList DatabaseManager::getAllDevices() {
    QStringList devices;
    QSqlQuery query("SELECT name FROM devices");

    if (query.exec()) {
        while (query.next()) {
            devices << query.value(0).toString();
        }
    } else {
        qDebug() << "Failed to retrieve devices: " << query.lastError().text();
    }
    return devices;
}

QStringList DatabaseManager::getAllRooms() {
    QStringList rooms;
    QSqlQuery query("SELECT name FROM rooms");

    if (query.exec()) {
        while (query.next()) {
            rooms << query.value(0).toString();
        }
    } else {
        qDebug() << "Failed to retrieve rooms: " << query.lastError().text();
    }

    return rooms;
}
QStringList DatabaseManager::getDevicesForRoom(const QString &roomName) {
    QStringList devices;
    QSqlQuery query;

    query.prepare("SELECT d.name FROM devices d JOIN rooms r ON d.room_id = r.id WHERE r.name = :roomName");
    query.bindValue(":roomName", roomName);

    if (query.exec()) {
        while (query.next()) {
            devices << query.value(0).toString();
        }
    } else {
        qDebug() << "Failed to retrieve devices for room" << roomName << ":" << query.lastError().text();
    }

    return devices;
}

QStringList DatabaseManager::getAllScenarios() {
    QStringList scenarios;
    QSqlQuery query("SELECT name FROM scenarios");

    if (query.exec()) {
        while (query.next()) {
            scenarios << query.value(0).toString();
        }
    } else {
        qDebug() << "Failed to retrieve scenarios: " << query.lastError().text();
    }
    return scenarios;
}

bool DatabaseManager::addScenario(const QString &scenario) {
    QSqlQuery query;
    query.prepare("INSERT INTO scenarios (name) VALUES (:name)");
    query.bindValue(":name", scenario);

    if (!query.exec()) {
        qDebug() << "Failed to add scenario: " << query.lastError().text();
        return false;
    }
    return true;
}
