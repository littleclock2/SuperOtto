#include "AStar.h"
#include <algorithm>

double AStar::heuristic(const Point& a, const Point& b) const {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<Point> AStar::findPath(const Grid& grid, Point start, Point goal) const {
    if (!grid.isInBounds(start.x, start.y) || !grid.isInBounds(goal.x, goal.y)) {
        return {};
    }
    if (grid.cellAt(start.x, start.y) == static_cast<int>(CellType::Obstacle) ||
        grid.cellAt(goal.x, goal.y) == static_cast<int>(CellType::Obstacle)) {
        return {};
    }

    std::priority_queue<std::pair<double, Point>,
                        std::vector<std::pair<double, Point>>,
                        std::greater<>> openSet;
    std::unordered_set<Point, PointHasher> closedSet;
    std::unordered_map<Point, Point, PointHasher> cameFrom;

    openSet.emplace(0, start);

    std::unordered_map<Point, double, PointHasher> gScore;
    gScore[start] = 0;

    std::unordered_map<Point, double, PointHasher> fScore;
    fScore[start] = heuristic(start, goal);

    while (!openSet.empty()) {
        Point current = openSet.top().second;
        openSet.pop();

        // Bug 4: Skip stale entries that were already processed
        if (closedSet.count(current)) continue;

        if (current == goal) {
            std::vector<Point> path;
            while (current != start) {
                path.push_back(current);
                current = cameFrom[current];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        }

        closedSet.insert(current);

        std::vector<Point> neighbors = {
            {current.x + 1, current.y},
            {current.x - 1, current.y},
            {current.x, current.y + 1},
            {current.x, current.y - 1}
        };

        for (const Point& neighbor : neighbors) {
            if (!grid.isInBounds(neighbor.x, neighbor.y) ||
                grid.cellAt(neighbor.x, neighbor.y) == static_cast<int>(CellType::Obstacle) ||
                closedSet.count(neighbor)) {
                continue;
            }

            double tentative_gScore = gScore[current] + 1;

            if (!gScore.count(neighbor) || tentative_gScore < gScore[neighbor]) {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentative_gScore;
                fScore[neighbor] = gScore[neighbor] + heuristic(neighbor, goal);

                openSet.emplace(fScore[neighbor], neighbor);
            }
        }
    }

    return {};
}
