#pragma once

#include <QWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QComboBox>

class SettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget* parent = nullptr);

    int rows() const;
    int cols() const;
    int obstacleNum() const;
    int range() const;
    bool useImages() const;
    QString resolution() const;

signals:
    void reloadRequested(int rows, int cols, int obstacleNum, int range);
    void imageModeChanged(bool useImages);
    void resolutionChanged(const QString& resolution);

private slots:
    void onReloadClicked();
    void onImageModeChanged(int state);
    void onResolutionChanged(int index);

private:
    QSpinBox* m_rowsSpin;
    QSpinBox* m_colsSpin;
    QSpinBox* m_obstacleSpin;
    QSpinBox* m_rangeSpin;
    QCheckBox* m_imageCheck;
    QComboBox* m_resolutionCombo;
    QPushButton* m_reloadBtn;
    QLabel* m_infoLabel;
};
