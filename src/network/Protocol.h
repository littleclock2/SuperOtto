#pragma once

#include <QString>
#include <QByteArray>
#include <vector>
#include "../core/Grid.h"

namespace Protocol {

enum class CommandType {
    Unknown,
    Path,
    Map,
    ReloadMap,
    Clear,
    Trophy,
    Win,
    Connected,
    Done,
    Chat
};

struct Message {
    CommandType type;
    QString raw;
    std::vector<Point> path;
    Point start;
    Point goal;
    std::vector<std::vector<int>> gridData;
    int obstacleNum;
    int range;
    int player;
    QString chatText;
    int x;
    int y;

    Message()
        : type(CommandType::Unknown)
        , obstacleNum(0)
        , range(0)
        , player(0)
        , x(0)
        , y(0)
        , start(0, 0)
        , goal(0, 0)
    {}
};

Message parse(const QByteArray& data);
QByteArray encode(const Message& msg);

QByteArray sendPath(const std::vector<Point>& path, Point start, Point goal);
QByteArray sendMap(const std::vector<std::vector<int>>& mapData, int obstacleNum, int range);
QByteArray sendChat(const QString& text);
QByteArray sendTrophy(int x, int y);
QByteArray sendWin(int player);
QByteArray sendReload(const std::vector<std::vector<int>>& mapData, int obstacleNum, int range);
QByteArray sendClear(int x, int y);
QByteArray sendConnected();
QByteArray sendDone();

} // namespace Protocol
