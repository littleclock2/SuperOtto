#pragma once
#include <afxwin.h>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <functional>  // For std::hash  
#include <cstddef>     // For std::size_t
#include <random>  

// 生成一个在[min, max]范围内的随机数  
int RandomNumber(int min, int max);

// 点结构体，用于表示坐标
struct Point {
    int x, y;
    Point(int x, int y) : x(x), y(y) {};
    Point() : x(0), y(0) {}  // 默认构造函数
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    };

    bool operator!=(const Point& other) const {
        return !(*this == other);
    };

    bool operator<(const Point& other)const {
        return x<other.x&&y<other.y;
    }
};


// 用于Point的哈希函数
struct PointHasher {
    std::size_t operator()(const Point& p) const {
        return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) <<1);
    }
};

double Heuristic(const Point& a, const Point& b);


std::vector<Point> AStar(const Point& start, const Point& goal, const std::vector<std::vector<int>>& grid);