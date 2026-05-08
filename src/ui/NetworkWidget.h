#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "../network/NetworkManager.h"

class NetworkWidget : public QWidget {
    Q_OBJECT
public:
    explicit NetworkWidget(NetworkManager* netManager, QWidget* parent = nullptr);

    QString getChatText() const;

signals:
    void connectRequested(const QString& ip, quint16 port, bool isHost);
    void disconnectRequested();
    void sendRequested(const QString& text);

private slots:
    void onConnectClicked();
    void onDisconnectClicked();
    void onSendClicked();
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(const QString& error);
    void onChatReceived(const QString& text);

private:
    NetworkManager* m_netManager;
    QLineEdit* m_ipEdit;
    QLineEdit* m_portEdit;
    QComboBox* m_roleCombo;
    QPushButton* m_connectBtn;
    QPushButton* m_disconnectBtn;
    QPushButton* m_sendBtn;
    QTextEdit* m_chatDisplay;
    QLineEdit* m_chatInput;
};
