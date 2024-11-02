#ifndef SMARTHOUSESERVER_H
#define SMARTHOUSESERVER_H


#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>


class SmartHouseSystemServer : public QTcpServer {
    Q_OBJECT
public:
    explicit SmartHouseSystemServer(QObject *parent = nullptr);
    bool startServer(quint16 port);

private slots:
    void processRegistrationRequest(QTcpSocket *socket, const QJsonObject &request);
    void processLoginRequest(QTcpSocket *socket, const QJsonObject &request);
    void processAddRoomRequest(QTcpSocket *socket, const QJsonObject &request);
    void processRoomLoadRequest(QTcpSocket *socket, const QJsonObject &request);
    void processLoadDevicesRequest(QTcpSocket *socket, const QJsonObject &request);
    void processLoadScenariosRequest(QTcpSocket *socket, const QJsonObject &request);
    void processAddScenarioRequest(QTcpSocket *socket, const QJsonObject &request);
    void processAddDeviceRequest(QTcpSocket *socket, const QJsonObject &request);
    void processRoomDevicesRequest(QTcpSocket *socket, const QJsonObject &request);

};
#endif // SMARTHOUSESERVER_H
