#pragma once

#include <vector>
#include <random>
#include <cstddef>

struct Point {
    int x, y;

    Point(int x, int y) : x(x), y(y) {}
    Point() : x(0), y(0) {}

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const {
        return !(*this == other);
    }

    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

struct PointHasher {
    std::size_t operator()(const Point& p) const {
        return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
    }
};

enum class CellType : int {
    Empty = 0,
    Obstacle = 1,
    Trophy = 2
};

class Grid {
public:
    Grid();
    Grid(int rows, int cols);

    int rows() const { return m_rows; }
    int cols() const { return m_cols; }

    int cellAt(int x, int y) const;
    void setCell(int x, int y, int value);

    void generateObstacles(int count, const Point& start1, const Point& start2,
                           const Point& goal1, const Point& goal2);
    Point placeTrophy(const Point& start1, const Point& start2);

    bool isInBounds(int x, int y) const;
    bool isEmpty(int x, int y) const;

    const std::vector<std::vector<int>>& data() const { return m_cells; }
    std::vector<std::vector<int>>& data() { return m_cells; }

    void setData(const std::vector<std::vector<int>>& cells);
    void resize(int rows, int cols);

private:
    int m_rows;
    int m_cols;
    std::vector<std::vector<int>> m_cells;
    std::mt19937 m_rng;
};
