#include "Game.h"
#include <QIcon>
#include <QPainter>
#include <QTimer>
#include "Plants.h"
#include <QKeyEvent>
#include "Map.h"
#include "Zombies.h"
#include <qrandom.h>
Plants *player;
Game::Game(QWidget *parent)
    : QWidget{parent}
{
    QTimer* timer = new QTimer(this);
    timer -> callOnTimeout(this, [=]()
    {
        //更新数据
        updateGame();
        //绘图
        update();
    });
        timer -> start(1000 / 60);//60fps
}

void Game::run()
{
    this -> show();
}

void Game::initGame(int w, int h, const QString &title, const QIcon &icon)
{
    setFixedSize(w, h);
    setWindowTitle(title);
    if(!icon.isNull())
    {
        setWindowIcon(icon);
    }
    WIDTH = w;
    HEIGHT = h;
    map = new Map;
    player = Mgr -> addEntity(new Plants(0,0,"Peashooter_0.png"));
}

void Game::cleanGame()
{
    delete Mgr;
}

void Game::updateGame()
{
    Mgr->refresh();
    map -> update();
    Mgr -> update();
    //限制移动范围
    if(player -> position.x() < 0)
    {
        player -> position.setX(0);
    }
    if(player -> position.x() > Game::WIDTH - player -> width())
    {
        player -> position.setX(Game::WIDTH - player -> width());
    }
    if(player -> position.y() < 0)
    {
        player -> position.setY(0);
    }
    if(player -> position.y() > Game::HEIGHT - player -> height())
    {
        player -> position.setY(Game::HEIGHT - player -> height());
    }
    static int i = 0;
    //发射子弹
    if(i % 30 == 0)
    {
        player -> shoot();
    }

    //僵尸出场
    if(i % 240 == 0)
    {
        QStringList es = {"Zombie_0.png", "Zombie_1.png", "Zombie_2.png", "Zombie_3.png", "Zombie_4.png",
                          "Zombie_5.png", "Zombie_6.png", "Zombie_7.png", "Zombie_8.png", "Zombie_9.png",
                          "Zombie_10.png", "Zombie_11.png", "Zombie_12.png", "Zombie_13.png", "Zombie_14.png",
                          "Zombie_15.png", "Zombie_16.png", "Zombie_17.png", "Zombie_18.png", "Zombie_19.png",
                          "Zombie_20.png","Zombie_21.png"};
        auto e = Mgr -> addEntity(new Zombies(1000,300,"Zombie_0.png"));
        e -> setPos(1500,QRandomGenerator::global() -> bounded(Game::HEIGHT) - e -> height());
        e -> speed = 1;
        e -> velocity.setX(-1);
    }


    i++;
}

void Game::drawGame(QPainter *painter)
{
    map -> draw(painter);
    Mgr -> draw(painter);
}

void Game::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    drawGame(&painter);
}

void Game::closeEvent(QCloseEvent *ev)
{
    cleanGame();
}

void Game::keyPressEvent(QKeyEvent *ev)
{
    switch (ev -> key()) {
    case Qt::Key_W:
        player -> velocity.setY(-1);
        break;
    case Qt::Key_S:
        player -> velocity.setY(1);
        break;
    case Qt::Key_A:
        player -> velocity.setX(-1);
        break;
    case Qt::Key_D:
        player -> velocity.setX(1);
        break;
    default:
        break;
    }

}

void Game::keyReleaseEvent(QKeyEvent *ev)
{
    switch (ev -> key()) {
    case Qt::Key_W:
        player -> velocity.setY(0);
        break;
    case Qt::Key_S:
        player -> velocity.setY(0);
        break;
    case Qt::Key_A:
        player -> velocity.setX(0);
        break;
    case Qt::Key_D:
        player -> velocity.setX(0);
        break;
    default:
        break;
    }
}









