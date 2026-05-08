#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "Protocol.h"

class NetworkManager : public QObject {
    Q_OBJECT
public:
    enum class Role {
        None,
        Host,
        Client
    };

    explicit NetworkManager(QObject* parent = nullptr);
    ~NetworkManager();

    void startHost(const QString& ip, quint16 port);
    void connectToHost(const QString& ip, quint16 port);
    void disconnect();
    void send(const QByteArray& data);

    bool isConnected() const;
    Role role() const { return m_role; }

signals:
    void connected();
    void disconnected();
    void messageReceived(const Protocol::Message& msg);
    void errorOccurred(const QString& error);
    void chatReceived(const QString& text);

private slots:
    void onNewConnection();
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QTcpServer* m_server;
    QTcpSocket* m_socket;
    Role m_role;
    QByteArray m_buffer;
};
