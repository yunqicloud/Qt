#include <QCoreApplication>
#include <QApplication>
#include "Game.h"


int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    QApplication a(argc, argv);

    Game game;
    game.initGame(1400, 600, "PvZ");
    game.run();

    return a.exec();
}
