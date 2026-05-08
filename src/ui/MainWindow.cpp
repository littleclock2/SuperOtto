#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_selectedX(-1)
    , m_selectedY(-1)
    , m_isHost(true)
    , m_useImages(true)
{
    setupUi();
    setupConnections();

    setWindowTitle(QString::fromUtf8("SuperOtto v3.2.0"));
    setMinimumSize(800, 600);
    resize(1960, 1200);

    QString appDir = QApplication::applicationDirPath();
    QString pictureDir;
    for (const QString& candidate : {appDir + "/../picturetest", appDir + "/picturetest"}) {
        if (QDir(candidate).exists()) { pictureDir = candidate; break; }
    }
    if (!pictureDir.isEmpty()) {
        m_gameWidget->loadAssets(pictureDir);
    }

    QString musicDir;
    for (const QString& candidate : {appDir + "/../music", appDir + "/music"}) {
        if (QDir(candidate).exists()) { musicDir = candidate; break; }
    }
    if (!musicDir.isEmpty()) {
        m_musicPlayer.loadFromDirectory(musicDir);
    }

    m_gameManager.newGame(30, 30, 200, 9);
    updateStatusDisplay();
    updateFontScale();
}

MainWindow::~MainWindow() {
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    updateFontScale();
}

void MainWindow::updateFontScale() {
    int refW = 1960;
    int refH = 1200;
    double scaleW = static_cast<double>(width()) / refW;
    double scaleH = static_cast<double>(height()) / refH;
    double scale = std::min(scaleW, scaleH);
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.0) scale = 2.0;

    int baseFs = 16;
    int fontSize = std::max(12, static_cast<int>(baseFs * scale));
    int bigFontSize = std::max(16, static_cast<int>(22 * scale));
    int sideFs = std::max(10, static_cast<int>(13 * scale));
    int scoreFs = std::max(14, static_cast<int>(20 * scale));
    int turnFs = std::max(18, static_cast<int>(28 * scale));

    QFont ctrlFont("Microsoft YaHei", fontSize);
    ctrlFont.setStyleStrategy(QFont::PreferAntialias);

    if (m_scoreLabel) {
        m_scoreLabel->setFont(QFont("Microsoft YaHei", scoreFs, QFont::Bold));
        m_scoreLabel->setStyleSheet(QString("font-weight: bold; font-size: %1px; color: #333;").arg(scoreFs));
    }
    if (m_scoreGroup) {
        m_scoreGroup->setFont(QFont("Microsoft YaHei", std::max(9, static_cast<int>(11 * scale)), QFont::Bold));
    }
    if (m_turnLabel) {
        m_turnLabel->setFont(QFont("Microsoft YaHei", turnFs, QFont::Bold));
    }
    if (m_rangeLabel) {
        m_rangeLabel->setFont(ctrlFont);
    }

    QFont sideFont("Microsoft YaHei", sideFs);
    sideFont.setStyleStrategy(QFont::PreferAntialias);

    if (m_settingsWidget) m_settingsWidget->setFont(sideFont);
    if (m_networkWidget) m_networkWidget->setFont(sideFont);
    if (m_musicWidget) m_musicWidget->setFont(sideFont);
}

void MainWindow::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    // ===== 游戏面板 =====
    QWidget* gamePanel = new QWidget();
    QVBoxLayout* gameLayout = new QVBoxLayout(gamePanel);
    gameLayout->setContentsMargins(0, 0, 0, 0);
    gameLayout->setSpacing(0);

    // 顶部控制栏
    QWidget* controlBar = new QWidget();
    QHBoxLayout* controlLayout = new QHBoxLayout(controlBar);
    controlLayout->setContentsMargins(8, 4, 8, 4);

    m_scoreGroup = new QGroupBox(QString::fromUtf8("得分"), this);
    m_scoreGroup->setStyleSheet(
        "QGroupBox { font-weight: bold; border: 2px solid #4CAF50; border-radius: 6px; "
        "margin-top: 10px; padding-top: 8px; font-size: 11px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; font-size: 11px; }");
    QHBoxLayout* scoreLayout = new QHBoxLayout(m_scoreGroup);
    scoreLayout->setContentsMargins(12, 6, 12, 6);
    m_scoreLabel = new QLabel(QString::fromUtf8("玩家1: 0  玩家2: 0"), m_scoreGroup);
    m_scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLayout->addWidget(m_scoreLabel);
    m_scoreGroup->setFixedWidth(280);
    controlLayout->addWidget(m_scoreGroup);

    m_rangeLabel = new QLabel(QString::fromUtf8("范围: 9"), this);
    controlLayout->addWidget(m_rangeLabel);

    controlLayout->addStretch();

    gameLayout->addWidget(controlBar);

    // 游戏画布
    m_gameWidget = new GameWidget(gamePanel);
    gameLayout->addWidget(m_gameWidget, 1);

    // 底部回合提示
    QWidget* bottomBar = new QWidget();
    bottomBar->setFixedHeight(50);
    bottomBar->setStyleSheet("background-color: rgba(0, 0, 0, 60);");
    QHBoxLayout* bottomLayout = new QHBoxLayout(bottomBar);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    m_turnLabel = new QLabel(QString::fromUtf8("玩家1 的回合"), bottomBar);
    m_turnLabel->setAlignment(Qt::AlignCenter);
    m_turnLabel->setStyleSheet("color: #228B22; font-weight: bold;");
    bottomLayout->addWidget(m_turnLabel);
    gameLayout->addWidget(bottomBar);

    m_mainSplitter->addWidget(gamePanel);

    // ===== 侧边栏 =====
    QWidget* sidePanel = new QWidget();
    QVBoxLayout* sideLayout = new QVBoxLayout(sidePanel);
    sideLayout->setContentsMargins(5, 5, 5, 5);

    m_settingsWidget = new SettingsWidget(sidePanel);
    sideLayout->addWidget(m_settingsWidget);

    QFrame* line1 = new QFrame(sidePanel);
    line1->setFrameShape(QFrame::HLine);
    sideLayout->addWidget(line1);

    m_networkWidget = new NetworkWidget(&m_networkManager, sidePanel);
    sideLayout->addWidget(m_networkWidget);

    QFrame* line2 = new QFrame(sidePanel);
    line2->setFrameShape(QFrame::HLine);
    sideLayout->addWidget(line2);

    m_musicWidget = new MusicWidget(&m_musicPlayer, sidePanel);
    sideLayout->addWidget(m_musicWidget);

    sideLayout->addStretch();
    m_mainSplitter->addWidget(sidePanel);

    m_mainSplitter->setStretchFactor(0, 3);
    m_mainSplitter->setStretchFactor(1, 1);
    m_mainSplitter->setSizes({1300, 620});

    mainLayout->addWidget(m_mainSplitter);
}

void MainWindow::setupConnections() {
    connect(m_gameWidget, &GameWidget::cellClicked, this, &MainWindow::onCellClicked);
    connect(m_gameWidget, &GameWidget::cellDoubleClicked, this, &MainWindow::onCellDoubleClicked);

    connect(m_settingsWidget, &SettingsWidget::reloadRequested, this, &MainWindow::onReloadMap);
    connect(m_settingsWidget, &SettingsWidget::imageModeChanged, this, &MainWindow::onImageModeChanged);
    connect(m_settingsWidget, &SettingsWidget::resolutionChanged, this, &MainWindow::onResolutionChanged);

    connect(m_networkWidget, &NetworkWidget::connectRequested, this, &MainWindow::onConnectRequested);
    connect(m_networkWidget, &NetworkWidget::disconnectRequested, this, &MainWindow::onDisconnectRequested);
    connect(m_networkWidget, &NetworkWidget::sendRequested, this, &MainWindow::onSendRequested);

    connect(&m_networkManager, &NetworkManager::messageReceived, this, &MainWindow::onNetworkMessageReceived);
    connect(&m_networkManager, &NetworkManager::connected, this, &MainWindow::onNetworkConnected);
    connect(&m_networkManager, &NetworkManager::disconnected, this, &MainWindow::onNetworkDisconnected);

    connect(&m_gameManager, &GameManager::turnChanged, this, &MainWindow::onTurnChanged);
    connect(&m_gameManager, &GameManager::scoreChanged, this, &MainWindow::onScoreChanged);
    connect(&m_gameManager, &GameManager::gridChanged, this, &MainWindow::onGridChanged);
    connect(&m_gameManager, &GameManager::trophyCollected, this, &MainWindow::onTrophyCollected);
}

void MainWindow::onCellClicked(int x, int y) {
    m_selectedX = x;
    m_selectedY = y;
    m_gameWidget->setSelectedCell(x, y);
}

void MainWindow::onCellDoubleClicked(int x, int y) {
    // 如果双击的是障碍物，执行清除障碍
    if (m_gameManager.grid().cellAt(x, y) == static_cast<int>(CellType::Obstacle)) {
        Point robotPos = m_gameManager.isPlayer1Turn()
            ? m_gameManager.robot1().position()
            : m_gameManager.robot2().position();

        if (m_gameManager.isObstacleValid(robotPos, Point(x, y))) {
            m_gameManager.clearObstacle(x, y);
            if (m_networkManager.isConnected()) {
                QByteArray data = Protocol::sendClear(x, y);
                m_networkManager.send(data);
            }
        } else {
            showToast(QString::fromUtf8("障碍物超出清除范围！"));
        }
        return;
    }

    // 否则作为目标寻路
    Point goal(x, y);
    Point robotPos = m_gameManager.isPlayer1Turn()
        ? m_gameManager.robot1().position()
        : m_gameManager.robot2().position();

    if (!m_gameManager.isGoalValid(robotPos, goal)) {
        showToast(QString::fromUtf8("目标超出移动范围！"));
        return;
    }

    m_gameManager.setGoal(x, y);
    m_gameManager.findPath();

    if (m_networkManager.isConnected()) {
        bool isP1 = m_gameManager.isPlayer1Turn();
        std::vector<Point> path = m_gameManager.currentPath();
        if (!path.empty()) {
            Point start = isP1 ? m_gameManager.start1() : m_gameManager.start2();
            Point goalPt = isP1 ? m_gameManager.goal1() : m_gameManager.goal2();
            QByteArray data = Protocol::sendPath(path, start, goalPt);
            m_networkManager.send(data);
        }
    }
}

void MainWindow::onReloadMap(int rows, int cols, int obstacleNum, int range) {
    m_gameManager.reloadMap(rows, cols, obstacleNum, range);

    if (m_networkManager.isConnected()) {
        QByteArray data = Protocol::sendReload(m_gameManager.gridToData(), obstacleNum, range);
        m_networkManager.send(data);
    }
}

void MainWindow::onImageModeChanged(bool useImages) {
    m_useImages = useImages;
    m_gameWidget->setUseImages(useImages);
    if (useImages) {
        QString appDir = QApplication::applicationDirPath();
        QString pictureDir;
        for (const QString& candidate : {appDir + "/../picturetest", appDir + "/picturetest"}) {
            if (QDir(candidate).exists()) { pictureDir = candidate; break; }
        }
        if (!pictureDir.isEmpty()) {
            m_gameWidget->loadAssets(pictureDir);
        }
    }
    m_gameWidget->update();
}

void MainWindow::onResolutionChanged(const QString& resolution) {
    if (resolution == QString::fromUtf8("自定义")) return;

    QStringList parts = resolution.split('x');
    if (parts.size() == 2) {
        int w = parts[0].toInt();
        int h = parts[1].toInt();
        if (w >= 800 && h >= 600) {
            resize(w, h);
        }
    }
}

void MainWindow::onConnectRequested(const QString& ip, quint16 port, bool isHost) {
    m_isHost = isHost;
    if (isHost) {
        m_networkManager.startHost(ip, port);
        m_gameManager.newGame(m_settingsWidget->rows(), m_settingsWidget->cols(),
                              m_settingsWidget->obstacleNum(), m_settingsWidget->range());
    } else {
        m_networkManager.connectToHost(ip, port);
    }
}

void MainWindow::onDisconnectRequested() {
    m_networkManager.disconnect();
}

void MainWindow::onSendRequested(const QString& text) {
    if (m_networkManager.isConnected()) {
        QByteArray data = Protocol::sendChat(text);
        m_networkManager.send(data);
    }
}

void MainWindow::onNetworkMessageReceived(const Protocol::Message& msg) {
    switch (msg.type) {
    case Protocol::CommandType::Path: {
        if (!msg.path.empty()) {
            m_gameManager.setGoal(msg.goal.x, msg.goal.y);
            m_gameManager.findPath();
        }
        break;
    }
    case Protocol::CommandType::Map:
    case Protocol::CommandType::ReloadMap: {
        if (!msg.gridData.empty()) {
            m_gameManager.setGridFromData(msg.gridData, msg.obstacleNum, msg.range);

            QByteArray done = Protocol::sendDone();
            m_networkManager.send(done);
        }
        break;
    }
    case Protocol::CommandType::Clear: {
        m_gameManager.grid().setCell(msg.x, msg.y, 0);
        m_gameManager.setTurn(!m_gameManager.isPlayer1Turn());
        m_gameWidget->setGrid(m_gameManager.grid());
        updateStatusDisplay();
        break;
    }
    case Protocol::CommandType::Trophy: {
        m_gameManager.grid().setCell(msg.x, msg.y, 2);
        m_gameWidget->setGrid(m_gameManager.grid());
        break;
    }
    case Protocol::CommandType::Win: {
        if (msg.player == 1) {
            showToast(QString::fromUtf8("玩家1 获得了奖杯！"));
        } else if (msg.player == 2) {
            showToast(QString::fromUtf8("玩家2 获得了奖杯！"));
        }
        break;
    }
    case Protocol::CommandType::Done: {
        updateStatusDisplay();
        break;
    }
    case Protocol::CommandType::Connected: {
        QByteArray mapData = Protocol::sendMap(m_gameManager.gridToData(),
                                               m_settingsWidget->obstacleNum(),
                                               m_settingsWidget->range());
        m_networkManager.send(mapData);
        break;
    }
    default:
        break;
    }
}

void MainWindow::onNetworkConnected() {
    if (m_isHost) {
        m_gameManager.newGame(m_settingsWidget->rows(), m_settingsWidget->cols(),
                              m_settingsWidget->obstacleNum(), m_settingsWidget->range());
    }
}

void MainWindow::onNetworkDisconnected() {
    updateStatusDisplay();
}

void MainWindow::onTurnChanged(bool isPlayer1Turn) {
    m_gameWidget->setPlayer1Turn(isPlayer1Turn);
    updateStatusDisplay();

    // 蓝色弹窗提示回合切换（3倍大小）
    QString turnText = isPlayer1Turn
        ? QString::fromUtf8("轮到 玩家1 行动")
        : QString::fromUtf8("轮到 玩家2 行动");
    showToast(turnText, QColor(30, 100, 200, 220));
}

void MainWindow::onScoreChanged(int score1, int score2) {
    m_scoreLabel->setText(QString::fromUtf8("玩家1: %1  玩家2: %2").arg(score1).arg(score2));
}

void MainWindow::onGridChanged() {
    m_gameWidget->setGrid(m_gameManager.grid());
    m_gameWidget->setRobots(m_gameManager.robot1(), m_gameManager.robot2());
    m_gameWidget->setStart1(m_gameManager.start1());
    m_gameWidget->setStart2(m_gameManager.start2());
    m_gameWidget->setGoal1(m_gameManager.goal1());
    m_gameWidget->setGoal2(m_gameManager.goal2());
    m_gameWidget->setRange(m_gameManager.currentRange());
    m_gameWidget->setPath(m_gameManager.currentPath());
    updateStatusDisplay();
}

void MainWindow::onTrophyCollected(int player) {
    if (m_networkManager.isConnected()) {
        Point trophy = m_gameManager.trophyPosition();
        QByteArray data = Protocol::sendTrophy(trophy.x, trophy.y);
        m_networkManager.send(data);

        QByteArray winData = Protocol::sendWin(player);
        m_networkManager.send(winData);
    }
}

void MainWindow::updateStatusDisplay() {
    int refW = 1960;
    double scale = std::max(0.6, std::min(2.0, static_cast<double>(width()) / refW));
    int turnFs = std::max(18, static_cast<int>(28 * scale));

    if (m_gameManager.isPlayer1Turn()) {
        m_turnLabel->setText(QString::fromUtf8("▶ 玩家1 的回合"));
        m_turnLabel->setStyleSheet(QString("color: #228B22; font-weight: bold; font-size: %1px;").arg(turnFs));
    } else {
        m_turnLabel->setText(QString::fromUtf8("▶ 玩家2 的回合"));
        m_turnLabel->setStyleSheet(QString("color: #8B008B; font-weight: bold; font-size: %1px;").arg(turnFs));
    }

    m_rangeLabel->setText(QString::fromUtf8("范围: %1").arg(m_gameManager.currentRange()));

    m_scoreLabel->setText(QString::fromUtf8("玩家1: %1  玩家2: %2").arg(m_gameManager.player1Score()).arg(m_gameManager.player2Score()));
}

void MainWindow::showToast(const QString& text, const QColor& bgColor) {
    if (!m_toastLabel) {
        m_toastLabel = new QLabel(m_gameWidget);
        m_toastLabel->setAlignment(Qt::AlignCenter);
        m_toastLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    }

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(m_toastLabel);
    m_toastLabel->setGraphicsEffect(effect);
    effect->setOpacity(1.0);

    int refW = 1960;
    double scale = std::max(0.6, std::min(2.0, static_cast<double>(m_gameWidget->width()) / refW));
    // 3倍大小的toast
    int toastFontSize = std::max(24, static_cast<int>(42 * scale));
    int paddingV = std::max(14, static_cast<int>(24 * scale));
    int paddingH = std::max(28, static_cast<int>(48 * scale));

    m_toastLabel->setText(text);
    m_toastLabel->setFont(QFont("Microsoft YaHei", toastFontSize, QFont::Bold));
    m_toastLabel->setStyleSheet(QString(
        "background-color: rgba(%1, %2, %3, %4); color: white; "
        "padding: %5px %6px; border-radius: 12px; font-size: %7px;")
        .arg(bgColor.red()).arg(bgColor.green()).arg(bgColor.blue()).arg(bgColor.alpha())
        .arg(paddingV).arg(paddingH).arg(toastFontSize));

    m_toastLabel->adjustSize();
    int x = (m_gameWidget->width() - m_toastLabel->width()) / 2;
    int y = (m_gameWidget->height() - m_toastLabel->height()) / 2;
    m_toastLabel->move(x, y);
    m_toastLabel->show();
    m_toastLabel->raise();

    QTimer::singleShot(2500, this, [this]() {
        if (m_toastLabel && m_toastLabel->isVisible()) {
            QGraphicsOpacityEffect* eff = qobject_cast<QGraphicsOpacityEffect*>(m_toastLabel->graphicsEffect());
            if (eff) {
                QPropertyAnimation* anim = new QPropertyAnimation(eff, "opacity");
                anim->setDuration(500);
                anim->setStartValue(1.0);
                anim->setEndValue(0.0);
                connect(anim, &QPropertyAnimation::finished, m_toastLabel, &QWidget::hide);
                anim->start(QAbstractAnimation::DeleteWhenStopped);
            }
        }
    });
}
