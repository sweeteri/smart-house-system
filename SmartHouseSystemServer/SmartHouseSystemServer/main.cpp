#include <QDebug>
#include "smarthousesystemserver.h"
#include <QCoreApplication>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    SmartHouseSystemServer server;
    return a.exec();
}
