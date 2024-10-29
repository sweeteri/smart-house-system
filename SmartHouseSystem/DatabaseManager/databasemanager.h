#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class DatabaseManager
{
public:
    static DatabaseManager& instance();

    bool openDatabase();
    void closeDatabase();
    QSqlDatabase getDatabase();

    bool authenticateUser(const QString &username, const QString &password);
    bool registerUser(const QString &username, const QString &password, const QString &role);

private:
    DatabaseManager();
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase db;
    bool ensureOpen();
    bool initializeTables();
};

#endif // DATABASEMANAGER_H
