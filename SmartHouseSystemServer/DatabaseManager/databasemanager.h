#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>

class DatabaseManager {
public:
    static DatabaseManager& instance() {
        static DatabaseManager instance;
        return instance;
    }

    bool registerUser(const QString &username, const QString &password, const QString &role);
    bool authenticateUser(const QString &username, const QString &password);
    bool userExists(const QString &username);
    bool openConnection();
    QSqlDatabase getDatabase();
    bool addRoom(const QString &roomName);
    bool addDevice(const QString &roomName, const QString &deviceName);
    QStringList getAllDevices();
    QStringList getAllRooms();
    QStringList getAllScenarios();
    QStringList getDevicesForRoom(const QString &roomName);
    bool addScenario(const QString &scenario);
    void initializeDatabase();
    QString getUserRole(const QString &username);
    bool adminExists();

private:
    DatabaseManager();
    ~DatabaseManager();
    QSqlDatabase db;
    DatabaseManager(DatabaseManager const&) = delete;
    void operator=(DatabaseManager const&) = delete;
};

#endif // DATABASEMANAGER_H
