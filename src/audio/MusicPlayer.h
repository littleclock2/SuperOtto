#pragma once

#include <QObject>
#include <QMediaPlayer>
#include <QUrl>
#include <QVector>

class MusicPlayer : public QObject {
    Q_OBJECT
public:
    explicit MusicPlayer(QObject* parent = nullptr);

    void play();
    void pause();
    void stop();
    void next();
    void previous();

    void addFile(const QString& filePath);
    void removeFile(int index);
    void loadFromDirectory(const QString& dirPath);

    int volume() const;
    void setVolume(int volume);

    int currentIndex() const { return m_currentIndex; }
    QString currentTitle() const;
    QStringList playlist() const { return m_titles; }
    bool isPlaying() const;

signals:
    void currentIndexChanged(int index);
    void titleChanged(const QString& title);
    void playlistChanged();
    void stateChanged(QMediaPlayer::State state);

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    QMediaPlayer* m_player;
    QVector<QUrl> m_urls;
    QStringList m_titles;
    int m_currentIndex;
};
