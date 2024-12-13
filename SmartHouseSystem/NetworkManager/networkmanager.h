#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QObject>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class NetworkManager : public QObject {
    Q_OBJECT

public:
    static NetworkManager& instance() {
        static NetworkManager instance;
        return instance;
    }

    bool connectToServer(const QString &host, quint16 port);

    void sendRequest(const QJsonObject &request);
    void sendHttpRequest(const QUrl &url, const QJsonObject &request);
    void sendGetRequest(const QUrl &url);

signals:
    void responseReceived(const QJsonObject &response);
    void httpResponseReceived(const QJsonObject &response);

private slots:
    void onConnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);
    void onHttpReplyFinished(QNetworkReply *reply);

private:
    QTcpSocket *socket;
    QNetworkAccessManager *httpManager;
    NetworkManager(QObject *parent = nullptr);
    ~NetworkManager() = default;


    NetworkManager(NetworkManager const&) = delete;
    void operator=(NetworkManager const&) = delete;
};

#endif // NETWORKMANAGER_H
