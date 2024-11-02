#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QObject>
#include <QDebug>

class NetworkManager : public QObject {
    Q_OBJECT

public:
    static NetworkManager& instance() {
        static NetworkManager instance;
        return instance;
    }

    bool connectToServer(const QString &host, quint16 port);

    void sendRequest(const QJsonObject &request);

signals:
    void responseReceived(const QJsonObject &response);

private slots:
    void onConnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *socket;

    NetworkManager(QObject *parent = nullptr);
    ~NetworkManager() = default;


    NetworkManager(NetworkManager const&) = delete;
    void operator=(NetworkManager const&) = delete;
};

#endif // NETWORKMANAGER_H
