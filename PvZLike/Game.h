#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QIcon>
#include <QPixmap>
#include <QRect>
#include "Manager.h"
#include "SoundManager.h"
#include <QElapsedTimer>

class Map;
class Qpainter;

// 游戏状态机
enum class GameState
{
    Menu,        // 主菜单(显示开始按钮)
    Playing,     // 游戏中
    Paused       // 暂停(菜单打开)
};

// 速度档位
enum class SpawnSpeed
{
    Slow   = 180,  // 慢:180 帧生成一只
    Normal = 120,  // 普通
    Fast   = 60    // 快
};

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
    void mousePressEvent(QMouseEvent*ev)override;
    void mouseMoveEvent(QMouseEvent*ev)override;  // 鼠标悬停高亮

private:
    Map *map;

    // UI 素材(本地路径加载)
    QPixmap m_mainMenuBg;
    QPixmap m_bigMenuBg;
    QPixmap m_startBtn;
    QPixmap m_startBtnHover;
    QPixmap m_menuBtn;        // 游戏中的小菜单按钮
    QPixmap m_menuBtnHover;
    QPixmap m_restartBtn;
    QPixmap m_restartBtnHover;
    QPixmap m_optionBtn;      // 暂停菜单里的 +/- 按钮
    QPixmap m_optionBtnHover;
    QPixmap m_returnBtn;       // 暂停菜单里的"返回游戏"按钮
    QPixmap m_returnBtnHover;

    // 状态机 + 计分
    GameState  m_state = GameState::Menu;
    int        m_score = 0;
    SpawnSpeed m_spawnSpeed = SpawnSpeed::Normal;
    bool       m_hoveredStart = false;
    bool       m_hoveredMenu = false;
    bool       m_hoveredRestart = false;
    bool       m_hoveredSpeedDown = false;
    bool       m_hoveredSpeedUp = false;
    bool       m_hoveredReturn = false;  // "返回游戏"按钮 hover 状态

    void initSound();
    void playSound(const QString& path);
    void playSound(SoundType type);

    // 按钮命中区域(每次 paint 前根据布局计算)
    QRect m_startBtnRect;
    QRect m_menuBtnRect;
    QRect m_restartBtnRect;
    QRect m_returnBtnRect;   // "返回游戏"按钮命中区(Paused 状态)
    QRect m_speedDownRect;
    QRect m_speedUpRect;
    QRect m_scorePanelRect;  // 计分板显示区(鼠标点它能开菜单)

    // 加载所有 UI 素材(从本地 R/ 路径)
    void loadUiAssets();

    // 业务方法
    void startNewGame();    // 重置分数/僵尸/玩家,进入 Playing
    void pauseGame();       // Playing -> Paused
    void resumeGame();      // Paused -> Playing
    void backToMainMenu();  // 任何状态 -> Menu
    void speedUp();         // spawn interval 减小
    void speedDown();       // spawn interval 增大

    void onClickStart();
    void onClickMenu();
    void onClickRestart();
    void onClickReturn();    // "返回游戏":Paused -> Playing
    void onClickSpeedUp();
    void onClickSpeedDown();

public:
    inline static int WIDTH = 0;
    inline static int HEIGHT = 0;
    inline static QElapsedTimer globalElapsedTimer;
};

#endif // GAME_H