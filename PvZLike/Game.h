#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QIcon>
#include "Manager.h"
#include "SoundManager.h"
#include <QElapsedTimer>

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
    void initSound();
    // 通用接口:传入 qrc 路径直接播放
    void playSound(const QString& path);
    // 类型安全接口:传入 SoundType 枚举(推荐)
    void playSound(SoundType type);

public:
    inline static int WIDTH = 0;
    inline static int HEIGHT = 0;
    inline static QElapsedTimer globalElapsedTimer;
};

#endif // GAME_H







