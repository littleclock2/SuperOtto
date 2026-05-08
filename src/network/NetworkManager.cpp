#include "NetworkManager.h"

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent)
    , m_server(nullptr)
    , m_socket(nullptr)
    , m_role(Role::None)
{
}

NetworkManager::~NetworkManager() {
    disconnect();
}

void NetworkManager::startHost(const QString& ip, quint16 port) {
    if (m_server) {
        m_server->deleteLater();
    }

    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &NetworkManager::onNewConnection);

    if (!m_server->listen(QHostAddress(ip), port)) {
        emit errorOccurred(m_server->errorString());
        return;
    }

    m_role = Role::Host;
}

void NetworkManager::connectToHost(const QString& ip, quint16 port) {
    if (m_socket) {
        m_socket->deleteLater();
    }

    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &NetworkManager::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkManager::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &NetworkManager::onError);

    m_role = Role::Client;
    m_socket->connectToHost(ip, port);
    // Bug 7 fix: async connect - error handling is done via the onError slot,
    // avoiding blocking the GUI thread with waitForConnected().
}

void NetworkManager::disconnect() {
    if (m_socket) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->waitForDisconnected(3000);
        }
        m_socket->deleteLater();
        m_socket = nullptr;
    }

    if (m_server) {
        m_server->close();
        m_server->deleteLater();
        m_server = nullptr;
    }

    m_role = Role::None;
    m_buffer.clear();
}

void NetworkManager::send(const QByteArray& data) {
    if (m_socket && m_socket->isWritable()) {
        m_socket->write(data);
        m_socket->flush();
    }
}

bool NetworkManager::isConnected() const {
    return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

void NetworkManager::onNewConnection() {
    if (!m_server) return;

    if (m_socket) {
        m_socket->deleteLater();
    }

    m_socket = m_server->nextPendingConnection();
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkManager::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &NetworkManager::onError);

    emit connected();
}

void NetworkManager::onConnected() {
    emit connected();
}

void NetworkManager::onDisconnected() {
    m_buffer.clear();
    emit disconnected();
}

void NetworkManager::onReadyRead() {
    if (!m_socket) return;

    m_buffer.append(m_socket->readAll());

    while (!m_buffer.isEmpty()) {
        // Bug 6 fix: handle multi-line messages (@path, @map/@Reload) that
        // span multiple newline-delimited lines and end with "@end".
        // Single-line messages use the first '\n' as delimiter.
        bool isMultiLine = m_buffer.trimmed().startsWith("@path")
                        || m_buffer.trimmed().startsWith("@map");

        if (isMultiLine) {
            // Wait until we see "@end" followed by a newline
            int endIdx = m_buffer.indexOf("@end");
            if (endIdx < 0) break; // incomplete message, wait for more data

            int afterEnd = m_buffer.indexOf('\n', endIdx);
            if (afterEnd < 0) break; // incomplete trailing data, wait for more

            QByteArray message = m_buffer.left(afterEnd + 1);
            m_buffer.remove(0, afterEnd + 1);

            Protocol::Message msg = Protocol::parse(message);
            if (msg.type != Protocol::CommandType::Unknown) {
                if (msg.type == Protocol::CommandType::Chat) {
                    emit chatReceived(msg.chatText);
                }
                emit messageReceived(msg);
            }
        } else {
            // Single-line message: delimiter is '\n'
            int nlIdx = m_buffer.indexOf('\n');
            if (nlIdx < 0) break; // no newline yet, wait for more data

            QByteArray line = m_buffer.left(nlIdx);
            m_buffer.remove(0, nlIdx + 1);

            Protocol::Message msg = Protocol::parse(line);
            if (msg.type != Protocol::CommandType::Unknown) {
                if (msg.type == Protocol::CommandType::Chat) {
                    emit chatReceived(msg.chatText);
                }
                emit messageReceived(msg);
            }
        }
    }
}

void NetworkManager::onError(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError);
    if (m_socket) {
        emit errorOccurred(m_socket->errorString());
        // Bug 8 fix: clean up socket on connection failure
        if (m_socket->state() == QAbstractSocket::UnconnectedState) {
            m_socket->deleteLater();
            m_socket = nullptr;
            m_role = Role::None;
        }
    }
}
