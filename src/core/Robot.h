#pragma once

#include "Grid.h"

class Robot {
public:
    Robot();
    Robot(int x, int y);

    Point position() const { return m_position; }
    void setPosition(const Point& pos) { m_position = pos; }
    void setPosition(int x, int y) { m_position = Point(x, y); }
    void moveTo(int x, int y) { m_position = Point(x, y); }

    int x() const { return m_position.x; }
    int y() const { return m_position.y; }

private:
    Point m_position;
};
