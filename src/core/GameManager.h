#pragma once

#include <QObject>
#include <vector>
#include <QTimer>
#include "Grid.h"
#include "Robot.h"
#include "AStar.h"

class GameManager : public QObject {
    Q_OBJECT
public:
    explicit GameManager(QObject* parent = nullptr);
    ~GameManager();

    void newGame(int rows, int cols, int obstacleNum, int range);
    void setGoal(int x, int y);
    void findPath();
    void clearObstacle(int x, int y);
    void reloadMap(int rows, int cols, int obstacleNum, int range);

    bool isPlayer1Turn() const { return m_player1Turn; }
    int player1Score() const { return m_player1Score; }
    int player2Score() const { return m_player2Score; }
    int currentRange() const { return m_range; }

    Grid& grid() { return m_grid; }
    const Grid& grid() const { return m_grid; }
    Robot& robot1() { return m_robot1; }
    Robot& robot2() { return m_robot2; }
    const std::vector<Point>& currentPath() const { return m_currentPath; }

    void setGridFromData(const std::vector<std::vector<int>>& data, int obstacleNum, int range);
    std::vector<std::vector<int>> gridToData() const;

    void moveRobot1AlongPath();
    void moveRobot2AlongPath();
    void checkTrophyCollected();

    Point start1() const { return m_start1; }
    Point start2() const { return m_start2; }
    Point goal1() const { return m_goal1; }
    Point goal2() const { return m_goal2; }
    Point trophyPosition() const { return m_trophyPos; }
    bool isGoalValid(const Point& robotPos, const Point& goal) const;
    bool isObstacleValid(const Point& robotPos, const Point& obstacle) const;

    void setTurn(bool player1) { m_player1Turn = player1; }

signals:
    void turnChanged(bool isPlayer1Turn);
    void scoreChanged(int score1, int score2);
    void pathFound(const std::vector<Point>& path);
    void trophyCollected(int player);
    void gameOver(int winner);
    void gridChanged();
    void robotMoved(int robotId, int x, int y);

private:
    void stopTimer(QTimer*& timer);
    void checkTrophyAtPosition(int x, int y);

    Grid m_grid;
    Robot m_robot1;
    Robot m_robot2;
    AStar m_astar;

    bool m_player1Turn;
    int m_player1Score;
    int m_player2Score;
    int m_range;
    std::vector<Point> m_currentPath;
    Point m_selectedGoal;

    Point m_start1;
    Point m_start2;
    Point m_goal1;
    Point m_goal2;
    Point m_trophyPos;

    int m_pathIndex;
    int m_obstacleNum;

    QTimer* m_robot1Timer = nullptr;
    QTimer* m_robot2Timer = nullptr;
};
