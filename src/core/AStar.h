#pragma once

#include "Grid.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <functional>

class AStar {
public:
    std::vector<Point> findPath(const Grid& grid, Point start, Point goal) const;

private:
    double heuristic(const Point& a, const Point& b) const;
};
