#include "Grid.h"
#include <queue>
#include <algorithm>
#include <random>

Grid::Grid()
    : m_rows(30)
    , m_cols(30)
    , m_cells(30, std::vector<int>(30, 0))
    , m_rng(std::random_device{}())
{
}

Grid::Grid(int rows, int cols)
    : m_rows(rows)
    , m_cols(cols)
    , m_cells(rows, std::vector<int>(cols, 0))
    , m_rng(std::random_device{}())
{
}

int Grid::cellAt(int x, int y) const {
    if (!isInBounds(x, y)) return -1;
    return m_cells[x][y];
}

void Grid::setCell(int x, int y, int value) {
    if (isInBounds(x, y)) {
        m_cells[x][y] = value;
    }
}

bool Grid::isInBounds(int x, int y) const {
    return x >= 0 && x < m_rows && y >= 0 && y < m_cols;
}

bool Grid::isEmpty(int x, int y) const {
    return isInBounds(x, y) && m_cells[x][y] == static_cast<int>(CellType::Empty);
}

void Grid::setData(const std::vector<std::vector<int>>& cells) {
    m_cells = cells;
    m_rows = static_cast<int>(cells.size());
    m_cols = m_rows > 0 ? static_cast<int>(cells[0].size()) : 0;
}

void Grid::resize(int rows, int cols) {
    m_rows = rows;
    m_cols = cols;
    m_cells.assign(rows, std::vector<int>(cols, 0));
}

static bool canReach(const std::vector<std::vector<int>>& grid, Point from, Point to) {
    int w = static_cast<int>(grid.size());
    int h = w > 0 ? static_cast<int>(grid[0].size()) : 0;
    if (from.x < 0 || from.x >= w || from.y < 0 || from.y >= h) return false;
    if (to.x < 0 || to.x >= w || to.y < 0 || to.y >= h) return false;
    if (grid[from.x][from.y] == 1 || grid[to.x][to.y] == 1) return false;

    std::vector<std::vector<bool>> visited(w, std::vector<bool>(h, false));
    std::queue<Point> q;
    visited[from.x][from.y] = true;
    q.push(from);
    const int dx4[] = {1, -1, 0, 0};
    const int dy4[] = {0, 0, 1, -1};

    while (!q.empty()) {
        Point cur = q.front();
        q.pop();
        if (cur.x == to.x && cur.y == to.y) return true;
        for (int d = 0; d < 4; d++) {
            int nx = cur.x + dx4[d];
            int ny = cur.y + dy4[d];
            if (nx >= 0 && nx < w && ny >= 0 && ny < h &&
                !visited[nx][ny] && grid[nx][ny] != 1) {
                visited[nx][ny] = true;
                q.push(Point(nx, ny));
            }
        }
    }
    return false;
}

void Grid::generateObstacles(int count, const Point& start1, const Point& start2,
                              const Point& goal1, const Point& goal2) {
    int maxAttempts = count * 10;
    int attempts = 0;
    int placed = 0;

    std::uniform_int_distribution<int> distX(0, m_rows - 1);
    std::uniform_int_distribution<int> distY(0, m_cols - 1);

    while (placed < count && attempts < maxAttempts) {
        attempts++;
        int x = distX(m_rng);
        int y = distY(m_rng);

        if (m_cells[x][y] != 0) continue;
        if (x == start1.x && y == start1.y) continue;
        if (x == start2.x && y == start2.y) continue;
        if (x == goal1.x && y == goal1.y) continue;
        if (x == goal2.x && y == goal2.y) continue;

        m_cells[x][y] = 1;
        bool ok = canReach(m_cells, start1, start2) &&
                  canReach(m_cells, start1, goal1) &&
                  canReach(m_cells, start2, goal2);
        if (ok) {
            placed++;
        } else {
            m_cells[x][y] = 0;
        }
    }
}

Point Grid::placeTrophy(const Point& start1, const Point& start2) {
    std::uniform_int_distribution<int> distX(0, m_rows - 1);
    std::uniform_int_distribution<int> distY(0, m_cols - 1);

    int maxAttempts = m_rows * m_cols * 2;
    int attempts = 0;
    Point trophy(-1, -1);

    do {
        trophy.x = distX(m_rng);
        trophy.y = distY(m_rng);
        attempts++;
        if (attempts >= maxAttempts) {
            return Point(-1, -1);
        }
    } while (m_cells[trophy.x][trophy.y] != 0 ||
             (trophy.x == start1.x && trophy.y == start1.y) ||
             (trophy.x == start2.x && trophy.y == start2.y) ||
             !canReach(m_cells, start1, trophy) ||
             !canReach(m_cells, start2, trophy));

    m_cells[trophy.x][trophy.y] = static_cast<int>(CellType::Trophy);
    return trophy;
}
