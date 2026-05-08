#include "GameWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <queue>

GameWidget::GameWidget(QWidget* parent)
    : QWidget(parent)
    , m_grid(30, 30)
    , m_robot1(0, 0)
    , m_robot2(29, 29)
    , m_range(9)
    , m_useImages(true)
    , m_start1(0, 0)
    , m_start2(29, 29)
    , m_goal1(0, 0)
    , m_goal2(29, 29)
    , m_selectedX(-1)
    , m_selectedY(-1)
    , m_cellSizeX(20)
    , m_cellSizeY(20)
    , m_player1Turn(true)
{
    setMinimumSize(200, 200);
    setMouseTracking(false);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(240, 248, 255));
    setPalette(pal);
}

void GameWidget::setGrid(const Grid& grid) {
    m_grid = grid;
    if (m_grid.rows() > 0 && m_grid.cols() > 0) {
        m_cellSizeX = width() / m_grid.rows();
        m_cellSizeY = height() / m_grid.cols();
    }
    update();
}

void GameWidget::setRobots(const Robot& robot1, const Robot& robot2) {
    m_robot1 = robot1;
    m_robot2 = robot2;
    update();
}

void GameWidget::setPath(const std::vector<Point>& path) {
    m_path = path;
    update();
}

void GameWidget::loadAssets(const QString& pictureDir) {
    QString dir = pictureDir;
    if (!dir.endsWith('/')) dir += '/';

    m_bgPixmap.load(dir + "bk.JPG");
    if (m_bgPixmap.isNull()) m_bgPixmap.load(dir + "bk.jpg");
    if (m_bgPixmap.isNull()) m_bgPixmap.load(dir + "bk.png");

    m_obstaclePixmap.load(dir + "Wall2.png");
    m_trophyPixmap.load(dir + "Cup.png");
    m_goalPixmap.load(dir + "Wall3.png");
    m_startPixmap.load(dir + "Wall4.png");
    m_robot1Pixmap.load(dir + "1.png");
    m_robot2Pixmap.load(dir + "2.png");
    m_pathPixmap.load(dir + "path.png");
    m_rangePixmap.load(dir + "range.png");

    if (m_bgPixmap.isNull()) {
        m_useImages = false;
    }
}

void GameWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_useImages && !m_bgPixmap.isNull()) {
        painter.drawPixmap(rect(), m_bgPixmap);
    } else {
        painter.fillRect(rect(), QColor(240, 248, 255));
    }

    if (m_grid.rows() == 0 || m_grid.cols() == 0) return;

    m_cellSizeX = width() / m_grid.rows();
    m_cellSizeY = height() / m_grid.cols();
    if (m_cellSizeX < 1) m_cellSizeX = 1;
    if (m_cellSizeY < 1) m_cellSizeY = 1;

    drawCells(painter);

    if (!m_useImages) {
        drawGridLines(painter);
    }

    drawRangeHighlight(painter);
    drawPaths(painter);
    drawGoals(painter);
    drawRobots(painter);

    if (m_selectedX >= 0 && m_selectedY >= 0) {
        drawSelectedCell(painter);
    }
}

void GameWidget::drawCells(QPainter& painter) {
    for (int i = 0; i < m_grid.rows(); ++i) {
        for (int j = 0; j < m_grid.cols(); ++j) {
            int cellValue = m_grid.cellAt(i, j);
            QRect cellRect(i * m_cellSizeX, j * m_cellSizeY, m_cellSizeX, m_cellSizeY);

            if (cellValue == static_cast<int>(CellType::Obstacle)) {
                if (m_useImages && !m_obstaclePixmap.isNull()) {
                    painter.drawPixmap(cellRect, m_obstaclePixmap);
                } else {
                    painter.fillRect(cellRect, QColor(64, 64, 64));
                }
            } else if (cellValue == static_cast<int>(CellType::Trophy)) {
                if (m_useImages && !m_trophyPixmap.isNull()) {
                    painter.drawPixmap(cellRect, m_trophyPixmap);
                } else {
                    painter.fillRect(cellRect, QColor(255, 215, 0));
                }
            }
        }
    }
}

void GameWidget::drawGridLines(QPainter& painter) {
    painter.setPen(QPen(QColor(200, 200, 200), 1));

    for (int i = 0; i <= m_grid.rows(); ++i) {
        painter.drawLine(i * m_cellSizeX, 0, i * m_cellSizeX, m_grid.cols() * m_cellSizeY);
    }
    for (int j = 0; j <= m_grid.cols(); ++j) {
        painter.drawLine(0, j * m_cellSizeY, m_grid.rows() * m_cellSizeX, j * m_cellSizeY);
    }
}

void GameWidget::drawPaths(QPainter& painter) {
    for (const Point& p : m_path) {
        QRect cellRect(p.x * m_cellSizeX, p.y * m_cellSizeY, m_cellSizeX, m_cellSizeY);
        if (m_useImages && !m_pathPixmap.isNull()) {
            painter.drawPixmap(cellRect, m_pathPixmap);
        } else {
            painter.fillRect(cellRect, QColor(135, 206, 235));
        }
    }
}

void GameWidget::drawRobots(QPainter& painter) {
    QRect r1Rect(m_robot1.x() * m_cellSizeX, m_robot1.y() * m_cellSizeY, m_cellSizeX, m_cellSizeY);
    if (m_useImages && !m_robot1Pixmap.isNull()) {
        painter.drawPixmap(r1Rect, m_robot1Pixmap);
    } else {
        painter.fillRect(r1Rect, QColor(255, 140, 0));
    }

    QRect r2Rect(m_robot2.x() * m_cellSizeX, m_robot2.y() * m_cellSizeY, m_cellSizeX, m_cellSizeY);
    if (m_useImages && !m_robot2Pixmap.isNull()) {
        painter.drawPixmap(r2Rect, m_robot2Pixmap);
    } else {
        painter.fillRect(r2Rect, QColor(148, 0, 211));
    }
}

void GameWidget::drawGoals(QPainter& painter) {
    QRect goal1Rect(m_goal1.x * m_cellSizeX, m_goal1.y * m_cellSizeY, m_cellSizeX, m_cellSizeY);
    if (m_useImages && !m_goalPixmap.isNull()) {
        painter.drawPixmap(goal1Rect, m_goalPixmap);
    } else {
        painter.fillRect(goal1Rect, QColor(220, 20, 60));
    }

    QRect goal2Rect(m_goal2.x * m_cellSizeX, m_goal2.y * m_cellSizeY, m_cellSizeX, m_cellSizeY);
    if (m_useImages && !m_goalPixmap.isNull()) {
        painter.drawPixmap(goal2Rect, m_goalPixmap);
    } else {
        painter.fillRect(goal2Rect, QColor(178, 34, 34));
    }

    QRect start1Rect(m_start1.x * m_cellSizeX, m_start1.y * m_cellSizeY, m_cellSizeX, m_cellSizeY);
    if (m_useImages && !m_startPixmap.isNull()) {
        painter.drawPixmap(start1Rect, m_startPixmap);
    } else {
        painter.fillRect(start1Rect, QColor(34, 139, 34));
    }

    QRect start2Rect(m_start2.x * m_cellSizeX, m_start2.y * m_cellSizeY, m_cellSizeX, m_cellSizeY);
    if (m_useImages && !m_startPixmap.isNull()) {
        painter.drawPixmap(start2Rect, m_startPixmap);
    } else {
        painter.fillRect(start2Rect, QColor(0, 100, 0));
    }
}

void GameWidget::drawRangeHighlight(QPainter& painter) {
    Point startPos = m_player1Turn ? m_robot1.position() : m_robot2.position();

    int gridW = m_grid.rows();
    int gridH = m_grid.cols();

    std::vector<std::vector<int>> dist(gridW, std::vector<int>(gridH, -1));
    std::queue<Point> q;
    dist[startPos.x][startPos.y] = 0;
    q.push(startPos);

    const int dx4[] = {1, -1, 0, 0};
    const int dy4[] = {0, 0, 1, -1};

    while (!q.empty()) {
        Point cur = q.front();
        q.pop();
        int curDist = dist[cur.x][cur.y];
        if (curDist >= m_range) continue;

        for (int d = 0; d < 4; d++) {
            int nx = cur.x + dx4[d];
            int ny = cur.y + dy4[d];
            if (nx >= 0 && nx < gridW && ny >= 0 && ny < gridH &&
                dist[nx][ny] == -1 && m_grid.cellAt(nx, ny) != static_cast<int>(CellType::Obstacle) &&
                m_grid.cellAt(nx, ny) != static_cast<int>(CellType::Trophy)) {
                dist[nx][ny] = curDist + 1;
                q.push(Point(nx, ny));
            }
        }
    }

    for (int i = 0; i < gridW; ++i) {
        for (int j = 0; j < gridH; ++j) {
            if (dist[i][j] >= 0) {
                QRect rect(i * m_cellSizeX, j * m_cellSizeY, m_cellSizeX, m_cellSizeY);
                if (m_useImages && !m_rangePixmap.isNull()) {
                    painter.drawPixmap(rect, m_rangePixmap);
                } else {
                    painter.fillRect(rect, QColor(175, 238, 238, 128));
                }
            }
        }
    }
}

void GameWidget::drawSelectedCell(QPainter& painter) {
    int centerX = m_selectedX * m_cellSizeX + m_cellSizeX / 2;
    int centerY = m_selectedY * m_cellSizeY + m_cellSizeY / 2;
    int radius = std::min(m_cellSizeX, m_cellSizeY) / 2;

    painter.setBrush(QColor(255, 236, 139));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPoint(centerX, centerY), radius, radius);
}

void GameWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int x = event->pos().x() / m_cellSizeX;
        int y = event->pos().y() / m_cellSizeY;

        if (x >= 0 && x < m_grid.rows() && y >= 0 && y < m_grid.cols()) {
            m_selectedX = x;
            m_selectedY = y;
            update();
            emit cellClicked(x, y);
        }
    }
}

void GameWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int x = event->pos().x() / m_cellSizeX;
        int y = event->pos().y() / m_cellSizeY;

        if (x >= 0 && x < m_grid.rows() && y >= 0 && y < m_grid.cols()) {
            m_selectedX = x;
            m_selectedY = y;
            update();
            emit cellDoubleClicked(x, y);
        }
    }
}

void GameWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (m_grid.rows() > 0 && m_grid.cols() > 0) {
        m_cellSizeX = event->size().width() / m_grid.rows();
        m_cellSizeY = event->size().height() / m_grid.cols();
    }
    update();
}
