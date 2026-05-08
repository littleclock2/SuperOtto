#pragma once

#include <QWidget>
#include <QPixmap>
#include <vector>
#include "../core/Grid.h"
#include "../core/Robot.h"

class GameWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameWidget(QWidget* parent = nullptr);

    void setGrid(const Grid& grid);
    void setRobots(const Robot& robot1, const Robot& robot2);
    void setPath(const std::vector<Point>& path);
    void setRange(int range) { m_range = range; }
    void setUseImages(bool use) { m_useImages = use; }
    void setStart1(const Point& p) { m_start1 = p; }
    void setStart2(const Point& p) { m_start2 = p; }
    void setGoal1(const Point& p) { m_goal1 = p; }
    void setGoal2(const Point& p) { m_goal2 = p; }
    void setSelectedCell(int x, int y) { m_selectedX = x; m_selectedY = y; }
    void setPlayer1Turn(bool p1) { m_player1Turn = p1; update(); }
    void loadAssets(const QString& pictureDir);

signals:
    void cellClicked(int x, int y);
    void cellDoubleClicked(int x, int y);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void drawCells(QPainter& painter);
    void drawPaths(QPainter& painter);
    void drawRobots(QPainter& painter);
    void drawGoals(QPainter& painter);
    void drawRangeHighlight(QPainter& painter);
    void drawSelectedCell(QPainter& painter);
    void drawGridLines(QPainter& painter);

    Grid m_grid;
    Robot m_robot1;
    Robot m_robot2;
    std::vector<Point> m_path;
    int m_range;
    bool m_useImages;
    Point m_start1;
    Point m_start2;
    Point m_goal1;
    Point m_goal2;
    int m_selectedX;
    int m_selectedY;

    QPixmap m_bgPixmap;
    QPixmap m_obstaclePixmap;
    QPixmap m_trophyPixmap;
    QPixmap m_goalPixmap;
    QPixmap m_startPixmap;
    QPixmap m_robot1Pixmap;
    QPixmap m_robot2Pixmap;
    QPixmap m_pathPixmap;
    QPixmap m_rangePixmap;

    int m_cellSizeX;
    int m_cellSizeY;
    bool m_player1Turn;
};
