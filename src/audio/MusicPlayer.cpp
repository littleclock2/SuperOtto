#include "MusicPlayer.h"
#include <QDir>
#include <QFileInfo>

MusicPlayer::MusicPlayer(QObject* parent)
    : QObject(parent)
    , m_player(new QMediaPlayer(this))
    , m_currentIndex(-1)
{
    m_player->setVolume(80);

    connect(m_player, &QMediaPlayer::mediaStatusChanged,
            this, &MusicPlayer::onMediaStatusChanged);
    connect(m_player, &QMediaPlayer::stateChanged,
            this, &MusicPlayer::stateChanged);
}

void MusicPlayer::play() {
    if (m_urls.isEmpty()) return;

    if (m_currentIndex < 0) {
        m_currentIndex = 0;
        emit currentIndexChanged(m_currentIndex);
    }

    if (m_player->state() == QMediaPlayer::PausedState) {
        m_player->play();
    } else {
        m_player->setMedia(QMediaContent(m_urls[m_currentIndex]));
        m_player->play();
    }

    emit titleChanged(currentTitle());
}

void MusicPlayer::pause() {
    if (m_player->state() == QMediaPlayer::PlayingState) {
        m_player->pause();
    } else if (m_player->state() == QMediaPlayer::PausedState) {
        m_player->play();
    }
}

void MusicPlayer::stop() {
    m_player->stop();
}

void MusicPlayer::next() {
    if (m_urls.isEmpty()) return;

    m_currentIndex = (m_currentIndex + 1) % m_urls.size();
    m_player->setMedia(QMediaContent(m_urls[m_currentIndex]));
    m_player->play();

    emit currentIndexChanged(m_currentIndex);
    emit titleChanged(currentTitle());
}

void MusicPlayer::previous() {
    if (m_urls.isEmpty()) return;

    m_currentIndex = (m_currentIndex - 1 + m_urls.size()) % m_urls.size();
    m_player->setMedia(QMediaContent(m_urls[m_currentIndex]));
    m_player->play();

    emit currentIndexChanged(m_currentIndex);
    emit titleChanged(currentTitle());
}

void MusicPlayer::addFile(const QString& filePath) {
    QFileInfo info(filePath);
    if (!info.exists()) return;

    m_urls.append(QUrl::fromLocalFile(filePath));
    m_titles.append(info.fileName());
    emit playlistChanged();
}

void MusicPlayer::removeFile(int index) {
    if (index < 0 || index >= m_urls.size()) return;

    m_urls.removeAt(index);
    m_titles.removeAt(index);

    if (m_currentIndex >= m_urls.size()) {
        m_currentIndex = m_urls.size() - 1;
    }

    emit playlistChanged();
}

void MusicPlayer::loadFromDirectory(const QString& dirPath) {
    QDir dir(dirPath);
    if (!dir.exists()) return;

    QStringList filters;
    filters << "*.mp3" << "*.wav" << "*.wma" << "*.ogg" << "*.flac";
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);

    for (const QFileInfo& file : files) {
        m_urls.append(QUrl::fromLocalFile(file.absoluteFilePath()));
        m_titles.append(file.fileName());
    }

    emit playlistChanged();
}

int MusicPlayer::volume() const {
    return m_player->volume();
}

void MusicPlayer::setVolume(int volume) {
    m_player->setVolume(volume);
}

QString MusicPlayer::currentTitle() const {
    if (m_currentIndex >= 0 && m_currentIndex < m_titles.size()) {
        return m_titles[m_currentIndex];
    }
    return QString();
}

bool MusicPlayer::isPlaying() const {
    return m_player->state() == QMediaPlayer::PlayingState;
}

void MusicPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        next();
    }
}
