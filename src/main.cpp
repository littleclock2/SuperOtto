#include <QApplication>
#include <QFont>
#include <QIcon>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("SuperOtto");
    app.setApplicationVersion("3.2.0");

    app.setWindowIcon(QIcon(":/icons/nailong.png"));

    QFont modernFont("Microsoft YaHei", 10);
    modernFont.setStyleStrategy(QFont::PreferAntialias);
    app.setFont(modernFont);

    MainWindow window;
    window.setWindowIcon(QIcon(":/icons/nailong.png"));
    window.show();

    return app.exec();
}
