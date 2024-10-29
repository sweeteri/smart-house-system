#include "loginwindow.h"
#include "databasemanager.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!DatabaseManager::instance().openDatabase()) {
        qDebug() << "Failed to open database. Exiting.";
        return -1;
    }
    qDebug() << "Database opened successfully";
    LoginWindow w;
    w.show();
    DatabaseManager::instance().closeDatabase();
    return a.exec();
}
