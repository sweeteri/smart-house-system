#include "networkmanager.h"
#include <QMessageBox>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent), socket(new QTcpSocket(this)),
                    httpManager(new QNetworkAccessManager(this))
{
    connect(socket, &QTcpSocket::connected, this, &NetworkManager::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);

    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onSocketError(QAbstractSocket::SocketError)));
    connect(httpManager, &QNetworkAccessManager::finished, this, &NetworkManager::onHttpReplyFinished);
}

bool NetworkManager::connectToServer(const QString &host, quint16 port) {
    socket->connectToHost(host, port);
    return socket->waitForConnected(5000);
}

void NetworkManager::sendRequest(const QJsonObject &request) {
    qDebug() << "Sending request to server:" << QJsonDocument(request).toJson();
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(QJsonDocument(request).toJson(QJsonDocument::Compact) + "\n");
        socket->flush();
        qDebug() << "Request sent successfully.";
    } else {
        qWarning() << "Not connected. Attempting to reconnect...";
    }
}

void NetworkManager::onConnected() {
    qDebug() << "Successfully connected to the server.";
}

void NetworkManager::onReadyRead() {
    QByteArray data = socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    qDebug() << "-----IBUSKO----- onReadyRead if (doc.isObject() " << doc.object();
    if (doc.isObject()) {
        qDebug() << "-----IBUSKO----- onReadyRead doc.object() " << doc.object();
        emit responseReceived(doc.object());
    }
}


void NetworkManager::onSocketError(QAbstractSocket::SocketError socketError) {
    if (socketError == QAbstractSocket::RemoteHostClosedError) {
        qDebug() << "Server closed the connection unexpectedly.";
    } else {
        qDebug() << "Socket error:" << socket->errorString();
    }
}
void NetworkManager::sendHttpRequest(const QUrl &url, const QJsonObject &request) {
    QNetworkRequest networkRequest(url);
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(request);
    QByteArray jsonData = doc.toJson();

    QNetworkReply *reply = httpManager->post(networkRequest, jsonData);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), [reply]() {
        qWarning() << "HTTP request error:" << reply->errorString();
    });
}
void NetworkManager::onHttpReplyFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        if (doc.isObject()) {
            emit httpResponseReceived(doc.object());
        }
    } else {
        qWarning() << "HTTP response error:" << reply->errorString();
    }
    reply->deleteLater();
}
void NetworkManager::sendGetRequest(const QUrl &url) {
    QNetworkRequest request(url);
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    // Отправляем GET-запрос
    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // Получаем ответ в формате JSON
            QByteArray responseData = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (!doc.isNull()) {
                QJsonObject jsonResponse = doc.object();
                emit responseReceived(jsonResponse);  // Отправляем полученный JSON
            } else {
                qWarning() << "Invalid JSON response";
            }
        } else {
            qWarning() << "Network error:" << reply->errorString();
        }
        reply->deleteLater();
    });
}

