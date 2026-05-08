#pragma once

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "../audio/MusicPlayer.h"

class MusicWidget : public QWidget {
    Q_OBJECT
public:
    explicit MusicWidget(MusicPlayer* player, QWidget* parent = nullptr);

private slots:
    void onPlayClicked();
    void onPauseClicked();
    void onStopClicked();
    void onNextClicked();
    void onPreviousClicked();
    void onAddClicked();
    void onRemoveClicked();
    void onVolumeChanged(int value);
    void onPlaylistDoubleClicked(int row);
    void onPlaylistChanged();
    void onTitleChanged(const QString& title);

private:
    MusicPlayer* m_player;
    QListWidget* m_listWidget;
    QPushButton* m_playBtn;
    QPushButton* m_pauseBtn;
    QPushButton* m_stopBtn;
    QPushButton* m_nextBtn;
    QPushButton* m_prevBtn;
    QPushButton* m_addBtn;
    QPushButton* m_removeBtn;
    QSlider* m_volumeSlider;
    QLabel* m_volumeLabel;
    QLabel* m_titleLabel;
};
