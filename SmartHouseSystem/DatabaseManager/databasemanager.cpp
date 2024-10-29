#include "databasemanager.h"
#include <QCryptographicHash>
#include <QMessageBox>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/Users/2005k/Documents/SmartHouseSystem/SmartHouseSystemServer/database.sqlite");
}

DatabaseManager::~DatabaseManager()
{
    closeDatabase();
}

bool DatabaseManager::openDatabase()
{
    if (!db.open()) {
        qDebug() << "Error: connection with database failed";
        qDebug() << db.lastError().text();
        return false;
    }
    qDebug() << "Database: connection ok";
    return true;
}

void DatabaseManager::closeDatabase()
{
    db.close();
}

QSqlDatabase DatabaseManager::getDatabase()
{
    return db;
}

bool DatabaseManager::ensureOpen()
{
    if (!db.isOpen()) {
        return openDatabase();
    }
    return true;
}

bool DatabaseManager::authenticateUser(const QString &username, const QString &password)
{
    if (!ensureOpen()) {
        qDebug() << "Failed to open database for authentication";
        return false;
    }

    QSqlQuery query(db);  // Make sure to pass 'db' here
    query.prepare("SELECT COUNT(*) FROM users WHERE username = ? AND password = ?");
    query.addBindValue(username);
    query.addBindValue(password);

    if (!query.exec()) {
        qDebug() << "Authentication query failed:" << query.lastError().text();
        qDebug() << "Query:" << query.lastQuery();
        qDebug() << "Error:" << query.lastError().text();
        return false;
    }

    if (query.next() && query.value(0).toInt() > 0) {
        return true;
    }
    return false;
}

bool DatabaseManager::registerUser(const QString &username, const QString &password, const QString &role)
{
    if (!ensureOpen()) {
        qDebug() << "Failed to open database for user registration";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, password, role) VALUES (:username, :password, :role)");
    query.addBindValue(username);
    query.addBindValue(password);
    query.addBindValue(role);

    if (!query.exec()) {
        qDebug() << "Failed to register user:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::initializeTables()
{
    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS rooms (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE NOT NULL)")) {
        qDebug() << "Failed to create rooms table:" << query.lastError().text();
        return false;
    }
    // Создание таблицы для устройств
    if (!query.exec("CREATE TABLE IF NOT EXISTS devices (id INTEGER PRIMARY KEY AUTOINCREMENT, room_id INTEGER, name TEXT NOT NULL, "
                    "FOREIGN KEY(room_id) REFERENCES rooms(id))")) {
        qDebug() << "Failed to create devices table:" << query.lastError().text();
        return false;
    }
    return true;
}
