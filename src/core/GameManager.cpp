#include "GameManager.h"
#include <QTimer>

GameManager::GameManager(QObject* parent)
    : QObject(parent)
    , m_grid(30, 30)
    , m_robot1(0, 0)
    , m_robot2(29, 29)
    , m_player1Turn(true)
    , m_player1Score(0)
    , m_player2Score(0)
    , m_range(9)
    , m_start1(0, 0)
    , m_start2(29, 29)
    , m_goal1(0, 0)
    , m_goal2(29, 29)
    , m_trophyPos(0, 0)
    , m_pathIndex(0)
    , m_obstacleNum(200)
    , m_robot1Timer(nullptr)
    , m_robot2Timer(nullptr)
{
}

GameManager::~GameManager() {
    stopTimer(m_robot1Timer);
    stopTimer(m_robot2Timer);
}

void GameManager::stopTimer(QTimer*& timer) {
    if (timer) {
        timer->stop();
        timer->deleteLater();
        timer = nullptr;
    }
}

void GameManager::newGame(int rows, int cols, int obstacleNum, int range) {
    m_grid.resize(rows, cols);
    m_range = range;
    m_obstacleNum = obstacleNum;
    m_player1Turn = true;
    m_player1Score = 0;
    m_player2Score = 0;
    m_currentPath.clear();

    m_start1 = Point(0, 0);
    m_start2 = Point(rows - 1, cols - 1);
    m_goal1 = m_start1;
    m_goal2 = m_start2;
    m_robot1.setPosition(m_start1);
    m_robot2.setPosition(m_start2);
    m_grid.generateObstacles(obstacleNum, m_start1, m_start2, m_goal1, m_goal2);
    m_trophyPos = m_grid.placeTrophy(m_start1, m_start2);

    emit gridChanged();
    emit turnChanged(m_player1Turn);
    emit scoreChanged(m_player1Score, m_player2Score);
}

void GameManager::setGoal(int x, int y) {
    Point goal(x, y);
    if (m_player1Turn) {
        if (isGoalValid(m_robot1.position(), goal)) {
            m_goal1 = goal;
            m_currentPath.clear();
        }
    } else {
        if (isGoalValid(m_robot2.position(), goal)) {
            m_goal2 = goal;
            m_currentPath.clear();
        }
    }
    emit gridChanged();
}

void GameManager::findPath() {
    if (m_player1Turn) {
        if (isObstacleValid(m_robot1.position(), m_goal1)) {
            return;
        }
        std::vector<Point> path = m_astar.findPath(m_grid, m_robot1.position(), m_goal1);
        if (!path.empty() && !isObstacleValid(m_robot1.position(), m_goal1)) {
            m_currentPath = path;
            m_start1 = m_robot1.position();
            moveRobot1AlongPath();
            emit pathFound(path);
        } else {
            // Bug 11: Switch turn on pathfinding failure
            m_player1Turn = false;
            emit turnChanged(m_player1Turn);
            emit gridChanged();
        }
    } else {
        if (isObstacleValid(m_robot2.position(), m_goal2)) {
            return;
        }
        std::vector<Point> path = m_astar.findPath(m_grid, m_robot2.position(), m_goal2);
        if (!path.empty()) {
            m_currentPath = path;
            m_start2 = m_robot2.position();
            moveRobot2AlongPath();
            emit pathFound(path);
        } else {
            // Bug 11: Switch turn on pathfinding failure
            m_player1Turn = true;
            emit turnChanged(m_player1Turn);
            emit gridChanged();
        }
    }
}

void GameManager::clearObstacle(int x, int y) {
    if (!m_grid.isInBounds(x, y)) return;

    Point obstacle(x, y);
    if (m_player1Turn) {
        if (isObstacleValid(m_robot1.position(), obstacle)) {
            m_grid.setCell(x, y, 0);
            m_player1Turn = false;
            emit turnChanged(m_player1Turn);
            emit gridChanged();
        }
    } else {
        if (isObstacleValid(m_robot2.position(), obstacle)) {
            m_grid.setCell(x, y, 0);
            m_player1Turn = true;
            emit turnChanged(m_player1Turn);
            emit gridChanged();
        }
    }
}

void GameManager::reloadMap(int rows, int cols, int obstacleNum, int range) {
    newGame(rows, cols, obstacleNum, range);
}

void GameManager::setGridFromData(const std::vector<std::vector<int>>& data, int obstacleNum, int range) {
    m_grid.setData(data);
    m_obstacleNum = obstacleNum;
    m_range = range;

    int rows = m_grid.rows();
    int cols = m_grid.cols();

    m_start1 = Point(0, 0);
    m_start2 = Point(rows - 1, cols - 1);
    m_goal1 = m_start1;
    m_goal2 = m_start2;
    m_robot1.setPosition(m_start1);
    m_robot2.setPosition(m_start2);
    m_player1Turn = true;
    m_player1Score = 0;
    m_player2Score = 0;
    m_currentPath.clear();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (m_grid.cellAt(i, j) == static_cast<int>(CellType::Trophy)) {
                m_trophyPos = Point(i, j);
            }
        }
    }

    emit gridChanged();
    emit turnChanged(m_player1Turn);
    emit scoreChanged(m_player1Score, m_player2Score);
}

std::vector<std::vector<int>> GameManager::gridToData() const {
    return m_grid.data();
}

void GameManager::moveRobot1AlongPath() {
    if (m_currentPath.empty()) return;

    // Bug 5: Stop and delete old timer before creating new one
    stopTimer(m_robot1Timer);

    // Bug 7: Start at index 1 to skip the start position
    m_pathIndex = 1;

    m_robot1Timer = new QTimer(this);
    QObject::connect(m_robot1Timer, &QTimer::timeout, this, [this]() {
        if (m_pathIndex < static_cast<int>(m_currentPath.size())) {
            Point p = m_currentPath[m_pathIndex];
            m_robot1.moveTo(p.x, p.y);
            m_pathIndex++;
            emit robotMoved(1, m_robot1.x(), m_robot1.y());

            // Bug 6: Check trophy at each step of the animation
            checkTrophyAtPosition(m_robot1.x(), m_robot1.y());

            emit gridChanged();
        } else {
            m_start1 = m_goal1;
            m_robot1.setPosition(m_goal1);
            m_currentPath.clear();
            stopTimer(m_robot1Timer);

            // Bug 13: Check trophy before switching turn
            checkTrophyAtPosition(m_robot1.x(), m_robot1.y());

            m_player1Turn = false;
            emit turnChanged(m_player1Turn);
            emit gridChanged();
        }
    });
    m_robot1Timer->start(300);
}

void GameManager::moveRobot2AlongPath() {
    if (m_currentPath.empty()) return;

    // Bug 5: Stop and delete old timer before creating new one
    stopTimer(m_robot2Timer);

    // Bug 7: Start at index 1 to skip the start position
    m_pathIndex = 1;

    m_robot2Timer = new QTimer(this);
    QObject::connect(m_robot2Timer, &QTimer::timeout, this, [this]() {
        if (m_pathIndex < static_cast<int>(m_currentPath.size())) {
            Point p = m_currentPath[m_pathIndex];
            m_robot2.moveTo(p.x, p.y);
            m_pathIndex++;
            emit robotMoved(2, m_robot2.x(), m_robot2.y());

            // Bug 6: Check trophy at each step of the animation
            checkTrophyAtPosition(m_robot2.x(), m_robot2.y());

            emit gridChanged();
        } else {
            m_start2 = m_goal2;
            m_robot2.setPosition(m_goal2);
            m_currentPath.clear();
            stopTimer(m_robot2Timer);

            // Bug 13: Check trophy before switching turn
            checkTrophyAtPosition(m_robot2.x(), m_robot2.y());

            m_player1Turn = true;
            emit turnChanged(m_player1Turn);
            emit gridChanged();
        }
    });
    m_robot2Timer->start(300);
}

void GameManager::checkTrophyAtPosition(int x, int y) {
    if (m_grid.cellAt(x, y) == static_cast<int>(CellType::Trophy)) {
        // Determine which player collected it based on current turn state
        // When called from moveRobot1AlongPath, it's player 1's turn
        // When called from moveRobot2AlongPath, it's player 2's turn
        int player = m_player1Turn ? 1 : 2;
        if (player == 1) {
            m_player1Score++;
        } else {
            m_player2Score++;
        }
        m_grid.setCell(x, y, 0);
        m_trophyPos = m_grid.placeTrophy(m_start1, m_start2);
        emit scoreChanged(m_player1Score, m_player2Score);
        emit trophyCollected(player);
        emit gridChanged();
    }
}

void GameManager::checkTrophyCollected() {
    if (m_grid.cellAt(m_start1.x, m_start1.y) == static_cast<int>(CellType::Trophy)) {
        m_player1Score++;
        m_grid.setCell(m_start1.x, m_start1.y, 0);
        m_trophyPos = m_grid.placeTrophy(m_start1, m_start2);
        emit scoreChanged(m_player1Score, m_player2Score);
        emit trophyCollected(1);
        emit gridChanged();
    } else if (m_grid.cellAt(m_start2.x, m_start2.y) == static_cast<int>(CellType::Trophy)) {
        m_player2Score++;
        m_grid.setCell(m_start2.x, m_start2.y, 0);
        m_trophyPos = m_grid.placeTrophy(m_start1, m_start2);
        emit scoreChanged(m_player1Score, m_player2Score);
        emit trophyCollected(2);
        emit gridChanged();
    }
}

bool GameManager::isGoalValid(const Point& robotPos, const Point& goal) const {
    if (!m_grid.isInBounds(goal.x, goal.y)) return false;
    if (m_grid.cellAt(goal.x, goal.y) == static_cast<int>(CellType::Obstacle)) return false;

    // Bug 9: Use const reference instead of deep copying the grid
    std::vector<Point> path = m_astar.findPath(m_grid, robotPos, goal);
    return (static_cast<int>(path.size()) <= m_range + 1 && !path.empty());
}

bool GameManager::isObstacleValid(const Point& robotPos, const Point& obstacle) const {
    int dx = std::abs(robotPos.x - obstacle.x);
    int dy = std::abs(robotPos.y - obstacle.y);
    return (dx <= m_range && dy <= m_range) &&
           m_grid.cellAt(obstacle.x, obstacle.y) == static_cast<int>(CellType::Obstacle);
}
