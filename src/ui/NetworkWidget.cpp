#include "NetworkWidget.h"

NetworkWidget::NetworkWidget(NetworkManager* netManager, QWidget* parent)
    : QWidget(parent)
    , m_netManager(netManager)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    QLabel* netLabel = new QLabel(QString::fromUtf8("网络"), this);
    netLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
    mainLayout->addWidget(netLabel);

    QHBoxLayout* roleLayout = new QHBoxLayout();
    QLabel* roleLabel = new QLabel(QString::fromUtf8("角色:"), this);
    m_roleCombo = new QComboBox(this);
    m_roleCombo->addItem(QString::fromUtf8("主机"));
    m_roleCombo->addItem(QString::fromUtf8("客户端"));
    roleLayout->addWidget(roleLabel);
    roleLayout->addWidget(m_roleCombo);
    mainLayout->addLayout(roleLayout);

    QHBoxLayout* ipLayout = new QHBoxLayout();
    QLabel* ipLabel = new QLabel(QString::fromUtf8("IP地址:"), this);
    m_ipEdit = new QLineEdit("127.0.0.1", this);
    ipLayout->addWidget(ipLabel);
    ipLayout->addWidget(m_ipEdit);
    mainLayout->addLayout(ipLayout);

    QHBoxLayout* portLayout = new QHBoxLayout();
    QLabel* portLabel = new QLabel(QString::fromUtf8("端口:"), this);
    m_portEdit = new QLineEdit("7600", this);
    portLayout->addWidget(portLabel);
    portLayout->addWidget(m_portEdit);
    mainLayout->addLayout(portLayout);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_connectBtn = new QPushButton(QString::fromUtf8("连接"), this);
    m_disconnectBtn = new QPushButton(QString::fromUtf8("断开"), this);
    m_disconnectBtn->setEnabled(false);
    btnLayout->addWidget(m_connectBtn);
    btnLayout->addWidget(m_disconnectBtn);
    mainLayout->addLayout(btnLayout);

    m_chatDisplay = new QTextEdit(this);
    m_chatDisplay->setReadOnly(true);
    m_chatDisplay->setMaximumHeight(120);
    mainLayout->addWidget(m_chatDisplay);

    QHBoxLayout* chatLayout = new QHBoxLayout();
    m_chatInput = new QLineEdit(this);
    m_sendBtn = new QPushButton(QString::fromUtf8("发送"), this);
    m_sendBtn->setEnabled(false);
    chatLayout->addWidget(m_chatInput);
    chatLayout->addWidget(m_sendBtn);
    mainLayout->addLayout(chatLayout);

    mainLayout->addStretch();

    connect(m_connectBtn, &QPushButton::clicked, this, &NetworkWidget::onConnectClicked);
    connect(m_disconnectBtn, &QPushButton::clicked, this, &NetworkWidget::onDisconnectClicked);
    connect(m_sendBtn, &QPushButton::clicked, this, &NetworkWidget::onSendClicked);
    connect(m_chatInput, &QLineEdit::returnPressed, this, &NetworkWidget::onSendClicked);

    connect(m_netManager, &NetworkManager::connected, this, &NetworkWidget::onConnected);
    connect(m_netManager, &NetworkManager::disconnected, this, &NetworkWidget::onDisconnected);
    connect(m_netManager, &NetworkManager::errorOccurred, this, &NetworkWidget::onErrorOccurred);
    connect(m_netManager, &NetworkManager::chatReceived, this, &NetworkWidget::onChatReceived);
}

void NetworkWidget::onConnectClicked() {
    QString ip = m_ipEdit->text().trimmed();
    quint16 port = m_portEdit->text().trimmed().toUShort();
    bool isHost = (m_roleCombo->currentIndex() == 0);

    emit connectRequested(ip, port, isHost);
}

void NetworkWidget::onDisconnectClicked() {
    emit disconnectRequested();
}

void NetworkWidget::onSendClicked() {
    QString text = m_chatInput->text().trimmed();
    if (text.isEmpty()) return;

    emit sendRequested(text);
    m_chatInput->clear();
}

void NetworkWidget::onConnected() {
    m_connectBtn->setEnabled(false);
    m_disconnectBtn->setEnabled(true);
    m_sendBtn->setEnabled(true);
    m_chatDisplay->append(QString::fromUtf8("已连接！"));
}

void NetworkWidget::onDisconnected() {
    m_connectBtn->setEnabled(true);
    m_disconnectBtn->setEnabled(false);
    m_sendBtn->setEnabled(false);
    m_chatDisplay->append(QString::fromUtf8("已断开。"));
}

void NetworkWidget::onErrorOccurred(const QString& error) {
    m_chatDisplay->append(QString::fromUtf8("错误: %1").arg(error));
}

void NetworkWidget::onChatReceived(const QString& text) {
    m_chatDisplay->append(QString::fromUtf8("对方: %1").arg(text));
}

QString NetworkWidget::getChatText() const {
    return m_chatDisplay->toPlainText();
}
