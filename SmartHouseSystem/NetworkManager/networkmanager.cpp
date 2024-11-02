#include "networkmanager.h"
#include <QMessageBox>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent), socket(new QTcpSocket(this))
{
    connect(socket, &QTcpSocket::connected, this, &NetworkManager::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);

    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onSocketError(QAbstractSocket::SocketError)));
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

