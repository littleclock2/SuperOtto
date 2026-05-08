#pragma once

#include <QMainWindow>
#include <QSplitter>
#include <QLabel>
#include <QGroupBox>
#include <QTimer>
#include <QDir>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

#include "../core/GameManager.h"
#include "../network/NetworkManager.h"
#include "../audio/MusicPlayer.h"
#include "GameWidget.h"
#include "MusicWidget.h"
#include "NetworkWidget.h"
#include "SettingsWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onCellClicked(int x, int y);
    void onCellDoubleClicked(int x, int y);
    void onReloadMap(int rows, int cols, int obstacleNum, int range);
    void onImageModeChanged(bool useImages);
    void onResolutionChanged(const QString& resolution);
    void onConnectRequested(const QString& ip, quint16 port, bool isHost);
    void onDisconnectRequested();
    void onSendRequested(const QString& text);
    void onNetworkMessageReceived(const Protocol::Message& msg);
    void onNetworkConnected();
    void onNetworkDisconnected();
    void onTurnChanged(bool isPlayer1Turn);
    void onScoreChanged(int score1, int score2);
    void onGridChanged();
    void onTrophyCollected(int player);

private:
    void setupUi();
    void setupConnections();
    void updateStatusDisplay();
    void showToast(const QString& text, const QColor& bgColor = QColor(0, 0, 0, 180));
    void updateFontScale();

    GameManager m_gameManager;
    NetworkManager m_networkManager;
    MusicPlayer m_musicPlayer;

    QSplitter* m_mainSplitter;
    GameWidget* m_gameWidget;
    MusicWidget* m_musicWidget;
    NetworkWidget* m_networkWidget;
    SettingsWidget* m_settingsWidget;

    QGroupBox* m_scoreGroup;
    QLabel* m_scoreLabel;
    QLabel* m_turnLabel;
    QLabel* m_rangeLabel;
    QLabel* m_toastLabel = nullptr;

    int m_selectedX;
    int m_selectedY;
    bool m_isHost;
    bool m_useImages;
};
