#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QIcon>
#include "Manager.h"
class Map;
class Qpainter;

class Game : public QWidget
{
    Q_OBJECT
public:
    explicit Game(QWidget *parent = nullptr);
    //运行游戏
    void run();
    //初始化游戏
    void initGame(int w, int h, const QString &title, const QIcon& = QIcon());
    //清理游戏
    void cleanGame();
    //更新游戏
    void updateGame();
    //绘制
    void drawGame(QPainter* painter);

protected:
    void paintEvent(QPaintEvent*ev)override;
    void closeEvent(QCloseEvent*ev)override;
    void keyPressEvent(QKeyEvent*ev)override;
    void keyReleaseEvent(QKeyEvent*ev)override;

private:
    Map *map;
public:
    inline static int WIDTH = 0;
    inline static int HEIGHT = 0;
};

#endif // GAME_H







