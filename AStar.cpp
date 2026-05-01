#include"pch.h"
#include "AStar.h"
int RandomNumber(int min, int max) {
    // 静态确保std::random_device只被调用一次  
    static std::random_device rd;  // 用于获取随机数种子  
    // 使用std::mt19937因为它是一个快速的随机数引擎  
    static std::mt19937 gen(rd()); // 使用random_device作为种子  
    // 定义随机数分布  
    std::uniform_int_distribution<> dis(min, max);

    // 生成随机数  
    return dis(gen);
}
// 计算两个点之间的曼哈顿距离（Heuristic函数）
double Heuristic(const Point& a, const Point& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);  // 曼哈顿距离
}
// A*算法实现，返回从起点到终点的最短路径
std::vector<Point> AStar(const Point& start, const Point& goal, const std::vector<std::vector<int>>& grid) {
    std::priority_queue<std::pair<double, Point>, std::vector<std::pair<double, Point>>, std::greater<>> openSet;
    std::unordered_set<Point, PointHasher> closedSet;
    std::unordered_map<Point, Point, PointHasher> cameFrom;

    openSet.emplace(0, start);

    std::unordered_map<Point, double, PointHasher> gScore;
    gScore[start] = 0;

    std::unordered_map<Point, double, PointHasher> fScore;
    fScore[start] = Heuristic(start, goal);

    while (!openSet.empty()) {
        Point current = openSet.top().second;
        openSet.pop();

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
            // 检查邻居是否在网格范围内且不是障碍物
            if (neighbor.x < 0 || neighbor.x >= grid.size() || neighbor.y < 0 || neighbor.y >= grid[0].size() || grid[neighbor.x][neighbor.y] == 1 || closedSet.count(neighbor)) {
                continue;
            }

            double tentative_gScore = gScore[current] + 1;

            if (!gScore.count(neighbor) || tentative_gScore < gScore[neighbor]) {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentative_gScore;
                fScore[neighbor] = gScore[neighbor] + Heuristic(neighbor, goal);

                openSet.emplace(fScore[neighbor], neighbor);
            }
        }
    }

    return {};  // 如果未找到路径，返回空路径
}