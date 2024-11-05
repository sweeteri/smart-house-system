#include "loginwindow.h"
#include "mainwindow.h"
#include "networkmanager.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    NetworkManager &networkManager = NetworkManager::instance();
    if (networkManager.connectToServer("127.0.0.1", 1234)) {
        qDebug() << "Connected to server!";
    } else {
        qWarning() << "Failed to connect to server.";
    }

    LoginWindow w;
    MainWindow* mainWindow = new MainWindow(nullptr);
    QObject::connect(&w, &LoginWindow::login_success, mainWindow, [&](const QString &role) {
        mainWindow->setUserRole(role);  // Set the role in MainWindow
        mainWindow->show();
    });
    /*QObject::connect(&w, &LoginWindow::login_success, mainWindow, [&]() {
        mainWindow->show();
        // Запросы на сервер о данных
        QJsonObject request;
        request["action"] = "loadRooms";
        qDebug() << "----IBUSKO---- request[action] = \"loadRooms\";";
        NetworkManager::instance().sendRequest(request);
    });*/
    QObject::connect(mainWindow, &MainWindow::backToMain, &w, [&w, mainWindow]() {
        w.showLoginWindow();
        mainWindow->hide();
    });
    w.show();

    return a.exec();
}
