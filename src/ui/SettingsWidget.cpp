#include "SettingsWidget.h"
#include <QGroupBox>
#include <QMessageBox>

SettingsWidget::SettingsWidget(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    QLabel* settingsLabel = new QLabel(QString::fromUtf8("地图设置"), this);
    settingsLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
    mainLayout->addWidget(settingsLabel);

    QGroupBox* mapGroup = new QGroupBox(QString::fromUtf8("地图配置"), this);
    QVBoxLayout* mapLayout = new QVBoxLayout(mapGroup);

    QHBoxLayout* rowLayout = new QHBoxLayout();
    QLabel* rowLabel = new QLabel(QString::fromUtf8("行数:"), this);
    m_rowsSpin = new QSpinBox(this);
    m_rowsSpin->setRange(3, 40);
    m_rowsSpin->setValue(30);
    rowLayout->addWidget(rowLabel);
    rowLayout->addWidget(m_rowsSpin);
    mapLayout->addLayout(rowLayout);

    QHBoxLayout* colLayout = new QHBoxLayout();
    QLabel* colLabel = new QLabel(QString::fromUtf8("列数:"), this);
    m_colsSpin = new QSpinBox(this);
    m_colsSpin->setRange(3, 40);
    m_colsSpin->setValue(30);
    colLayout->addWidget(colLabel);
    colLayout->addWidget(m_colsSpin);
    mapLayout->addLayout(colLayout);

    QHBoxLayout* obsLayout = new QHBoxLayout();
    QLabel* obsLabel = new QLabel(QString::fromUtf8("障碍物:"), this);
    m_obstacleSpin = new QSpinBox(this);
    m_obstacleSpin->setRange(0, 500);
    m_obstacleSpin->setValue(200);
    obsLayout->addWidget(obsLabel);
    obsLayout->addWidget(m_obstacleSpin);
    mapLayout->addLayout(obsLayout);

    QHBoxLayout* rangeLayout = new QHBoxLayout();
    QLabel* rangeLabel = new QLabel(QString::fromUtf8("范围:"), this);
    m_rangeSpin = new QSpinBox(this);
    m_rangeSpin->setRange(1, 40);
    m_rangeSpin->setValue(9);
    rangeLayout->addWidget(rangeLabel);
    rangeLayout->addWidget(m_rangeSpin);
    mapLayout->addLayout(rangeLayout);

    mainLayout->addWidget(mapGroup);

    QGroupBox* displayGroup = new QGroupBox(QString::fromUtf8("显示设置"), this);
    QVBoxLayout* displayLayout = new QVBoxLayout(displayGroup);

    m_imageCheck = new QCheckBox(QString::fromUtf8("使用图片素材"), this);
    m_imageCheck->setChecked(true);
    displayLayout->addWidget(m_imageCheck);

    QHBoxLayout* resLayout = new QHBoxLayout();
    QLabel* resLabel = new QLabel(QString::fromUtf8("分辨率:"), this);
    m_resolutionCombo = new QComboBox(this);
    m_resolutionCombo->addItem("800x600");
    m_resolutionCombo->addItem("1024x768");
    m_resolutionCombo->addItem("1280x720");
    m_resolutionCombo->addItem("1600x900");
    m_resolutionCombo->addItem("1920x1080");
    m_resolutionCombo->addItem(QString::fromUtf8("自定义"));
    m_resolutionCombo->setCurrentIndex(4);
    resLayout->addWidget(resLabel);
    resLayout->addWidget(m_resolutionCombo);
    displayLayout->addLayout(resLayout);

    mainLayout->addWidget(displayGroup);

    m_reloadBtn = new QPushButton(QString::fromUtf8("重新生成地图"), this);
    m_reloadBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; font-weight: bold; font-size: 14px; }");
    mainLayout->addWidget(m_reloadBtn);

    m_infoLabel = new QLabel(QString::fromUtf8("点击\"重新生成\"以生成新地图"), this);
    m_infoLabel->setStyleSheet("color: #666; font-size: 12px;");
    mainLayout->addWidget(m_infoLabel);

    mainLayout->addStretch();

    connect(m_reloadBtn, &QPushButton::clicked, this, &SettingsWidget::onReloadClicked);
    connect(m_imageCheck, &QCheckBox::stateChanged, this, &SettingsWidget::onImageModeChanged);
    connect(m_resolutionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsWidget::onResolutionChanged);
}

int SettingsWidget::rows() const { return m_rowsSpin->value(); }
int SettingsWidget::cols() const { return m_colsSpin->value(); }
int SettingsWidget::obstacleNum() const { return m_obstacleSpin->value(); }
int SettingsWidget::range() const { return m_rangeSpin->value(); }
bool SettingsWidget::useImages() const { return m_imageCheck->isChecked(); }
QString SettingsWidget::resolution() const { return m_resolutionCombo->currentText(); }

void SettingsWidget::onReloadClicked() {
    int r = m_rowsSpin->value();
    int c = m_colsSpin->value();
    int obs = m_obstacleSpin->value();
    int rng = m_rangeSpin->value();

    int maxObstacles = std::max(1, r * c * 3 / 10);
    if (obs > maxObstacles) {
        QMessageBox::warning(this, QString::fromUtf8("警告"),
            QString::fromUtf8("障碍物过多！建议最多 %1 个，已自动调整。").arg(maxObstacles));
        m_obstacleSpin->setValue(maxObstacles);
        obs = maxObstacles;
    }

    if (rng > r || rng > c) {
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("范围超出地图大小！"));
        return;
    }

    emit reloadRequested(r, c, obs, rng);
}

void SettingsWidget::onImageModeChanged(int state) {
    emit imageModeChanged(state == Qt::Checked);
}

void SettingsWidget::onResolutionChanged(int index) {
    Q_UNUSED(index);
    emit resolutionChanged(m_resolutionCombo->currentText());
}
