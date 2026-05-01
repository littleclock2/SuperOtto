#pragma once
#include <vector>
#include "AStar.h"
// �������࣬���ڱ�ʾ�����˼���λ��
class Robot {
    public:
        Point position;
        Robot() : position(0, 0) {};
        Robot(int x, int y) : position(x, y) {};
        //Robot(int startX, int startY) : x(startX), y(startY) {};
        
        void SetPosition(int x, int y) {
            position.x = x;
            position.y = y;
        };
        // ���û����˵�λ��
        void MoveTo(int newX, int newY) {
            position.x = newX;
            position.y = newY;
        };
        
        // ��ȡ�����˵�X����
        int GetX() const { return position.x; };
        // ��ȡ�����˵�Y����
        int GetY() const { return position.y; };
};