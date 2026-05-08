#include "Protocol.h"
#include <QTextStream>
#include <QStringList>

namespace Protocol {

Message parse(const QByteArray& data) {
    Message msg;
    msg.raw = QString::fromUtf8(data);
    QString str = msg.raw;

    if (str.startsWith("@path")) {
        msg.type = CommandType::Path;
        QStringList lines = str.mid(6).split('\n', QString::SkipEmptyParts);
        if (!lines.isEmpty()) {
            QStringList coords = lines[0].split(' ', QString::SkipEmptyParts);
            if (coords.size() >= 4) {
                msg.start = Point(coords[0].toInt(), coords[1].toInt());
                msg.goal = Point(coords[2].toInt(), coords[3].toInt());
            }
            // Bug 4: if first path point is on the same line as start/goal coords
            int pathLineStart = 1;
            if (coords.size() >= 6) {
                msg.path.push_back(Point(coords[4].toInt(), coords[5].toInt()));
            }
            for (int i = pathLineStart; i < lines.size(); ++i) {
                if (lines[i].trimmed() == "@end") break;
                QStringList p = lines[i].split(' ', QString::SkipEmptyParts);
                if (p.size() >= 2) {
                    msg.path.push_back(Point(p[0].toInt(), p[1].toInt()));
                }
            }
        }
    } else if (str.startsWith("@map")) {
        msg.type = CommandType::Map;
        QString content = str.mid(5);
        if (content.startsWith("@Reload")) {
            msg.type = CommandType::ReloadMap;
            content = content.mid(8); // skip "@Reload\n"

            // Bug 1&2 fix: read obstacleNum and range from the first two
            // newline-separated lines after "@Reload"
            int nl1 = content.indexOf('\n');
            int nl2 = (nl1 >= 0) ? content.indexOf('\n', nl1 + 1) : -1;

            if (nl1 >= 0) {
                msg.obstacleNum = content.left(nl1).trimmed().toInt();
            }
            if (nl2 >= 0) {
                msg.range = content.mid(nl1 + 1, nl2 - nl1 - 1).trimmed().toInt();
            }

            // Grid parsing: start AFTER the obstacleNum/range lines (after second \n)
            int endIdx = content.indexOf("@end");
            if (endIdx >= 0 && nl2 >= 0) {
                QString gridStr = content.mid(nl2 + 1, endIdx - nl2 - 1);
                QStringList rows = gridStr.split("//", QString::SkipEmptyParts);
                for (const QString& row : rows) {
                    QStringList cells = row.split(' ', QString::SkipEmptyParts);
                    std::vector<int> rowData;
                    for (const QString& cell : cells) {
                        if (!cell.trimmed().isEmpty()) {
                            rowData.push_back(cell.toInt());
                        }
                    }
                    if (!rowData.empty()) {
                        msg.gridData.push_back(rowData);
                    }
                }
            }
        } else {
            // Regular @map (non-Reload) parsing
            int endIdx = content.indexOf("@end");
            if (endIdx >= 0) {
                QString gridStr = content.left(endIdx);
                QStringList rows = gridStr.split("//", QString::SkipEmptyParts);
                for (const QString& row : rows) {
                    QStringList cells = row.split(' ', QString::SkipEmptyParts);
                    std::vector<int> rowData;
                    for (const QString& cell : cells) {
                        if (!cell.trimmed().isEmpty()) {
                            rowData.push_back(cell.toInt());
                        }
                    }
                    if (!rowData.empty()) {
                        msg.gridData.push_back(rowData);
                    }
                }

                QString afterEnd = content.mid(endIdx + 4).trimmed();
                QStringList endParams = afterEnd.split('\n', QString::SkipEmptyParts);
                if (!endParams.isEmpty()) {
                    QStringList nums = endParams[0].split(' ', QString::SkipEmptyParts);
                    if (nums.size() >= 2) {
                        msg.obstacleNum = nums[0].toInt();
                        msg.range = nums[1].toInt();
                    }
                }
            }
        }
    } else if (str.startsWith("@clear")) {
        msg.type = CommandType::Clear;
        QStringList parts = str.mid(7).split(' ', QString::SkipEmptyParts);
        if (parts.size() >= 2) {
            msg.x = parts[0].toInt();
            msg.y = parts[1].toInt();
        }
    } else if (str.startsWith("@Trophy")) {
        msg.type = CommandType::Trophy;
        QStringList parts = str.mid(8).split(' ', QString::SkipEmptyParts);
        if (parts.size() >= 2) {
            msg.x = parts[0].toInt();
            msg.y = parts[1].toInt();
        }
    } else if (str.startsWith("@win")) {
        msg.type = CommandType::Win;
        if (str.size() > 5) {
            QChar c = str[5];
            if (c == '1') msg.player = 1;
            else if (c == '2') msg.player = 2;
        }
    } else if (str.startsWith("@connected")) {
        msg.type = CommandType::Connected;
    } else if (str.startsWith("@DONE")) {
        msg.type = CommandType::Done;
    } else {
        msg.type = CommandType::Chat;
        msg.chatText = str;
    }

    return msg;
}

QByteArray encode(const Message& msg) {
    switch (msg.type) {
    case CommandType::Path: {
        QByteArray result;
        QTextStream stream(&result);
        stream << "@path\n";
        stream << msg.start.x << " " << msg.start.y << " "
               << msg.goal.x << " " << msg.goal.y << "\n";
        for (const Point& p : msg.path) {
            stream << p.x << " " << p.y << "\n";
        }
        stream << "@end\n";
        stream.flush();
        return result;
    }
    case CommandType::Map: {
        QByteArray result;
        QTextStream stream(&result);
        stream << "@map\n";
        for (const auto& row : msg.gridData) {
            for (int cell : row) {
                stream << cell << " ";
            }
            stream << "// ";
        }
        stream << "@end " << msg.obstacleNum << " " << msg.range << "\n";
        stream.flush();
        return result;
    }
    case CommandType::ReloadMap: {
        QByteArray result;
        QTextStream stream(&result);
        stream << "@map\n@Reload\n";
        stream << msg.obstacleNum << "\n" << msg.range << "\n";
        for (const auto& row : msg.gridData) {
            for (int cell : row) {
                stream << cell << " ";
            }
            stream << "// ";
        }
        stream << "@end " << msg.obstacleNum << " " << msg.range << "\n";
        stream.flush();
        return result;
    }
    case CommandType::Clear:
        return QString("@clear\n%1 %2\n").arg(msg.x).arg(msg.y).toUtf8();
    case CommandType::Trophy:
        return QString("@Trophy\n%1 %2\n").arg(msg.x).arg(msg.y).toUtf8();
    case CommandType::Win:
        return QString("@win %1\n").arg(msg.player).toUtf8();
    case CommandType::Connected:
        return "@connected\n";
    case CommandType::Done:
        return "@DONE\n";
    case CommandType::Chat:
        return (msg.chatText + "\n").toUtf8();
    default:
        return QByteArray();
    }
}

QByteArray sendPath(const std::vector<Point>& path, Point start, Point goal) {
    Message msg;
    msg.type = CommandType::Path;
    msg.path = path;
    msg.start = start;
    msg.goal = goal;
    return encode(msg);
}

QByteArray sendMap(const std::vector<std::vector<int>>& mapData, int obstacleNum, int range) {
    Message msg;
    msg.type = CommandType::Map;
    msg.gridData = mapData;
    msg.obstacleNum = obstacleNum;
    msg.range = range;
    return encode(msg);
}

QByteArray sendChat(const QString& text) {
    Message msg;
    msg.type = CommandType::Chat;
    msg.chatText = text;
    return encode(msg);
}

QByteArray sendTrophy(int x, int y) {
    Message msg;
    msg.type = CommandType::Trophy;
    msg.x = x;
    msg.y = y;
    return encode(msg);
}

QByteArray sendWin(int player) {
    Message msg;
    msg.type = CommandType::Win;
    msg.player = player;
    return encode(msg);
}

QByteArray sendReload(const std::vector<std::vector<int>>& mapData, int obstacleNum, int range) {
    Message msg;
    msg.type = CommandType::ReloadMap;
    msg.gridData = mapData;
    msg.obstacleNum = obstacleNum;
    msg.range = range;
    return encode(msg);
}

QByteArray sendClear(int x, int y) {
    Message msg;
    msg.type = CommandType::Clear;
    msg.x = x;
    msg.y = y;
    return encode(msg);
}

QByteArray sendConnected() {
    Message msg;
    msg.type = CommandType::Connected;
    return encode(msg);
}

QByteArray sendDone() {
    Message msg;
    msg.type = CommandType::Done;
    return encode(msg);
}

} // namespace Protocol
