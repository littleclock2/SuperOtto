#include "MusicWidget.h"
#include <QFileDialog>
#include <QStandardPaths>

MusicWidget::MusicWidget(MusicPlayer* player, QWidget* parent)
    : QWidget(parent)
    , m_player(player)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    QLabel* musicLabel = new QLabel(QString::fromUtf8("音乐播放器"), this);
    musicLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
    mainLayout->addWidget(musicLabel);

    m_titleLabel = new QLabel(QString::fromUtf8("未播放"), this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("color: #333; font-size: 14px;");
    mainLayout->addWidget(m_titleLabel);

    m_listWidget = new QListWidget(this);
    m_listWidget->setMaximumHeight(120);
    mainLayout->addWidget(m_listWidget);

    QHBoxLayout* controlLayout = new QHBoxLayout();
    m_prevBtn = new QPushButton(tr("<<"), this);
    m_playBtn = new QPushButton(QString::fromUtf8("播放"), this);
    m_pauseBtn = new QPushButton(QString::fromUtf8("暂停"), this);
    m_stopBtn = new QPushButton(QString::fromUtf8("停止"), this);
    m_nextBtn = new QPushButton(tr(">>"), this);

    m_prevBtn->setFixedWidth(50);
    m_playBtn->setFixedWidth(60);
    m_pauseBtn->setFixedWidth(60);
    m_stopBtn->setFixedWidth(60);
    m_nextBtn->setFixedWidth(50);

    controlLayout->addWidget(m_prevBtn);
    controlLayout->addWidget(m_playBtn);
    controlLayout->addWidget(m_pauseBtn);
    controlLayout->addWidget(m_stopBtn);
    controlLayout->addWidget(m_nextBtn);
    mainLayout->addLayout(controlLayout);

    QHBoxLayout* volumeLayout = new QHBoxLayout();
    QLabel* volIcon = new QLabel(QString::fromUtf8("音量:"), this);
    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(80);
    m_volumeLabel = new QLabel("80%", this);
    m_volumeLabel->setFixedWidth(35);

    volumeLayout->addWidget(volIcon);
    volumeLayout->addWidget(m_volumeSlider);
    volumeLayout->addWidget(m_volumeLabel);
    mainLayout->addLayout(volumeLayout);

    QHBoxLayout* fileLayout = new QHBoxLayout();
    m_addBtn = new QPushButton(QString::fromUtf8("添加"), this);
    m_removeBtn = new QPushButton(QString::fromUtf8("移除"), this);
    fileLayout->addWidget(m_addBtn);
    fileLayout->addWidget(m_removeBtn);
    mainLayout->addLayout(fileLayout);

    mainLayout->addStretch();

    connect(m_playBtn, &QPushButton::clicked, this, &MusicWidget::onPlayClicked);
    connect(m_pauseBtn, &QPushButton::clicked, this, &MusicWidget::onPauseClicked);
    connect(m_stopBtn, &QPushButton::clicked, this, &MusicWidget::onStopClicked);
    connect(m_nextBtn, &QPushButton::clicked, this, &MusicWidget::onNextClicked);
    connect(m_prevBtn, &QPushButton::clicked, this, &MusicWidget::onPreviousClicked);
    connect(m_addBtn, &QPushButton::clicked, this, &MusicWidget::onAddClicked);
    connect(m_removeBtn, &QPushButton::clicked, this, &MusicWidget::onRemoveClicked);
    connect(m_volumeSlider, &QSlider::valueChanged, this, &MusicWidget::onVolumeChanged);
    connect(m_listWidget, &QListWidget::doubleClicked, this, [this](const QModelIndex& index) {
        Q_UNUSED(index);
        m_player->play();
    });

    connect(m_player, &MusicPlayer::playlistChanged, this, &MusicWidget::onPlaylistChanged);
    connect(m_player, &MusicPlayer::titleChanged, this, &MusicWidget::onTitleChanged);

    m_player->setVolume(80);
}

void MusicWidget::onPlayClicked() {
    m_player->play();
}

void MusicWidget::onPauseClicked() {
    m_player->pause();
}

void MusicWidget::onStopClicked() {
    m_player->stop();
}

void MusicWidget::onNextClicked() {
    m_player->next();
}

void MusicWidget::onPreviousClicked() {
    m_player->previous();
}

void MusicWidget::onAddClicked() {
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        QString::fromUtf8("选择音乐文件"),
        QStandardPaths::writableLocation(QStandardPaths::MusicLocation),
        QString::fromUtf8("音频文件 (*.mp3 *.wav *.wma *.ogg *.flac);;所有文件 (*.*)")
    );

    for (const QString& file : files) {
        m_player->addFile(file);
    }
}

void MusicWidget::onRemoveClicked() {
    int row = m_listWidget->currentRow();
    if (row >= 0) {
        m_player->removeFile(row);
    }
}

void MusicWidget::onVolumeChanged(int value) {
    m_player->setVolume(value);
    m_volumeLabel->setText(QString("%1%").arg(value));
}

void MusicWidget::onPlaylistDoubleClicked(int row) {
    Q_UNUSED(row);
    m_player->play();
}

void MusicWidget::onPlaylistChanged() {
    m_listWidget->clear();
    for (const QString& title : m_player->playlist()) {
        m_listWidget->addItem(title);
    }
}

void MusicWidget::onTitleChanged(const QString& title) {
    m_titleLabel->setText(title.isEmpty() ? QString::fromUtf8("未播放") : title);
}
