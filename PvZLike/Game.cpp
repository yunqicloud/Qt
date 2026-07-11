#include "Game.h"
#include <QIcon>
#include <QPainter>
#include <QTimer>
#include <QDateTime>
#include <QMouseEvent>
#include <QFont>
#include <QFontMetrics>
#include <QCoreApplication>
#include <QFile>
#include "Plants.h"
#include <QKeyEvent>
#include "Map.h"
#include "Zombies.h"
#include <qrandom.h>
#include "Global.h"
#include "bullet.h"
#include "SoundManager.h"



Plants *player;

// 项目内资源根目录。CMake 在编译时把 PVL_R_DIR 展开成源码 R/ 的绝对路径,
// 所有图片/音频都从项目内 R/ 加载,完全不依赖运行时拷贝。
static QString rRoot()
{
#ifdef PVL_R_DIR
    return QString(PVL_R_DIR);
#else
    return QCoreApplication::applicationDirPath() + "/R";
#endif
}
static QString uiAssetPath(const QString& rel)
{
    return rRoot() + "/graphics/Screen/" + rel;
}
static QString soundAssetPath(const QString& rel)
{
    return rRoot() + "/sound/" + rel;
}

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
    globalElapsedTimer.start();

    // 加载 UI 按钮/菜单素材(本地路径,避免依赖 qrc)
    loadUiAssets();

    map = new Map;
    player = Mgr -> addEntity(new Plants(250,300,"graphics/Plants/Peashooter/Peashooter_0.png"));
    player -> setType(EntityType::ET_Player);

    auto plantsAnim = player -> addAnimation("normal",new Animation);
    plantsAnim -> addFrame("graphics/Plants/Peashooter/Peashooter_0.png");
    plantsAnim -> addFrame("graphics/Plants/Peashooter/Peashooter_1.png");
    plantsAnim -> addFrame("graphics/Plants/Peashooter/Peashooter_2.png");
    plantsAnim -> addFrame("graphics/Plants/Peashooter/Peashooter_3.png");
    plantsAnim -> addFrame("graphics/Plants/Peashooter/Peashooter_4.png");
    plantsAnim -> addFrame("graphics/Plants/Peashooter/Peashooter_5.png");
    plantsAnim -> addFrame("graphics/Plants/Peashooter/Peashooter_6.png");
    plantsAnim -> addFrame("graphics/Plants/Peashooter/Peashooter_7.png");
    plantsAnim -> addFrame("graphics/Plants/Peashooter/Peashooter_8.png");
    plantsAnim -> addFrame("graphics/Plants/Peashooter/Peashooter_9.png");
    plantsAnim -> addFrame("graphics/Plants/Peashooter/Peashooter_10.png");
    plantsAnim -> addFrame("graphics/Plants/Peashooter/Peashooter_11.png");
    plantsAnim -> addFrame("graphics/Plants/Peashooter/Peashooter_12.png");
    player -> runAnimation("normal");

    // 启动音效系统并循环播放战斗 BGM
    initSound();

    // 进入主菜单状态(显示开始按钮)
    m_state = GameState::Menu;
    m_score = 0;
    m_spawnSpeed = SpawnSpeed::Normal;

}

void Game::loadUiAssets()
{
    m_mainMenuBg.load(uiAssetPath("MainMenu.png"));
    m_bigMenuBg.load(uiAssetPath("bigMenu.png"));
    //m_startBtn.load(uiAssetPath("StartButton.png"));Adventure_1.png
    m_startBtn.load(uiAssetPath("Adventure_1.png"));
    m_startBtnHover.load(uiAssetPath("StartButton.png"));   // 同一张,hover 走 tint 边框
    m_menuBtn.load(uiAssetPath("littleMenu.png"));
    m_menuBtnHover.load(uiAssetPath("littleMenu.png"));
    m_restartBtn.load(uiAssetPath("restartButton.png"));
    m_restartBtnHover.load(uiAssetPath("restartButton.png"));
    m_optionBtn.load(uiAssetPath("volumeButton.png"));
    m_optionBtnHover.load(uiAssetPath("volumeButton.png"));
    // "返回游戏"按钮:透明背景文字 sprite,放在暂停菜单里
    m_returnBtn.load(uiAssetPath("return.png"));
    m_returnBtnHover.load(uiAssetPath("return.png"));
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

    // 菜单/暂停状态下不更新游戏逻辑(僵尸不动、不生成新僵尸、不发射子弹)
    if (m_state != GameState::Playing)
    {
        i++;  // 仍然累加,这样恢复游戏时生成节奏不突兀
        return;
    }

    //发射子弹
    if(i % 45 == 0)
    {
        player -> shoot();
    }

    //僵尸出场(节奏由菜单里的 +/- 按钮控制)
    if(i > 0 && i % static_cast<int>(m_spawnSpeed) == 0)
    {
        auto e = Mgr -> addEntity(new Zombies(2000,2000,"graphics/Zombies/NormalZombie/Zombie/Zombie_0.png"));
        int randY = QRandomGenerator::global()->bounded(Game::HEIGHT - e->height());
        e -> setPos(1450,randY);
        e -> speed = 1;
        e -> velocity.setX(-1);
        e -> setType(EntityType::ET_Zombies);
        Animation* walkAnim = e->addAnimation("normal", new Animation);
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_0.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_1.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_2.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_3.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_4.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_5.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_6.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_7.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_8.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_9.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_10.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_11.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_12.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_13.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_14.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_15.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_16.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_17.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_18.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_19.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_20.png");
        walkAnim -> addFrame("graphics/Zombies/NormalZombie/Zombie/Zombie_21.png");

        e->runAnimation("normal");

        Animation* dieAnim = e->addAnimation("die", new Animation(120));
        dieAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieDie/ZombieDie_0.png");
        dieAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieDie/ZombieDie_1.png");
        dieAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieDie/ZombieDie_2.png");
        dieAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieDie/ZombieDie_3.png");
        dieAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieDie/ZombieDie_4.png");
        dieAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieDie/ZombieDie_5.png");
        dieAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieDie/ZombieDie_6.png");
        dieAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieDie/ZombieDie_7.png");
        dieAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieDie/ZombieDie_8.png");
        dieAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieDie/ZombieDie_9.png");

        Animation* attackAnim = e->addAnimation("attack", new Animation(120));
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_0.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_1.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_2.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_3.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_4.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_5.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_6.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_7.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_8.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_9.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_10.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_11.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_12.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_13.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_14.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_15.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_16.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_17.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_18.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_19.png");
        attackAnim -> addFrame("graphics/Zombies/NormalZombie/ZombieAttack/ZombieAttack_20.png");
    }

    //碰撞检测
    for(auto e : Mgr -> getEntityByType<Zombies>(EntityType::ET_Zombies))
    {
        // 如果僵尸已经在死亡动画中,跳过碰撞 + 玩家互动逻辑,
        // 否则每帧都重复触发 die,堆积 QTimer::singleShot 队列
        // (这会导致音效重复 + 僵尸不释放)
        if (e->dying) continue;

        bool isDead = false;
        //子弹与僵尸的碰撞
        for(auto b : Mgr -> getEntityByType<Bullet>(EntityType::ET_Bullet))
        {
            if (e -> collider.intersects(b -> collider))
            {
                b->runAnimation("explode");
                QTimer::singleShot(80, [=](){
                    b->destroy();
                });

                // 豌豆爆炸声
                playSound(SoundType::BulletExplode);

                e->runAnimation("die");
                e->velocity.setX(0);  // 死掉后停止移动
                e->dying = true;  // 标记为死亡中,本帧后续及未来帧跳过
                QTimer::singleShot(300, [=](){
                    e->destroy();
                });

                // 一个僵尸 10 分
                m_score += 10;

                isDead = true;
                break;
            }
        }
        //玩家与僵尸的碰撞
        if (!isDead && player->collider.intersects(e->collider))
        {
            e->velocity.setX(0);
            e->runAnimation("attack");
            // 僵尸啃咬声:节流到每 120ms 最多播一次,避免 60fps 持续重播造成爆音
            // (多只僵尸同时啃咬时也保留"咬"的节奏感)
            static qint64 lastAttackSoundMs = 0;
            qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
            if (nowMs - lastAttackSoundMs >= 120)
            {
                playSound(SoundType::ZombieAttack);
                lastAttackSoundMs = nowMs;
            }
        }
        else
        {
            if(e->getCurrAnimationName() != "die")
            {
                e->velocity.setX(-1);
                e->runAnimation("normal");
            }
        }

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

    if (m_state == GameState::Menu)
    {
        // 主菜单:显示 MainMenu 背景 + StartButton
        if (!m_mainMenuBg.isNull())
            painter.drawPixmap(0, 0, WIDTH, HEIGHT, m_mainMenuBg);
        else
        {
            // 兜底:背景全黑
            painter.fillRect(rect(), Qt::black);
        }

        // 计算开始按钮位置(屏幕中央偏下一点)
        const int btnW = m_startBtn.width();
        const int btnH = m_startBtn.height();
        m_startBtnRect = QRect((WIDTH - btnW) / 2 + 250,
                               HEIGHT / 2 - 200,
                               btnW, btnH);

        QPixmap& btn = m_hoveredStart ? m_startBtnHover : m_startBtn;
        if (!btn.isNull())
            painter.drawPixmap(m_startBtnRect, btn);
    }
    else if (m_state == GameState::Playing)
    {
        // 游戏画面
        drawGame(&painter);

        // 左上角计分板
        QFont scoreFont = painter.font();
        scoreFont.setPointSize(20);
        scoreFont.setBold(true);
        painter.setFont(scoreFont);
        painter.setPen(QColor(255, 220, 60));  // 黄色
        painter.setBrush(QColor(0, 0, 0, 160));
        m_scorePanelRect = QRect(10, 10, 240, 50);
        painter.drawRoundedRect(m_scorePanelRect, 8, 8);
        painter.drawText(m_scorePanelRect, Qt::AlignCenter,
                         QString("分数: %1").arg(m_score));

        // 菜单按钮(在计分板右边)
        const int menuBtnW = m_menuBtn.width();
        const int menuBtnH = m_menuBtn.height();
        m_menuBtnRect = QRect(m_scorePanelRect.right() + 10, 10,
                               menuBtnW, menuBtnH);
        QPixmap& mb = m_hoveredMenu ? m_menuBtnHover : m_menuBtn;
        if (!mb.isNull())
            painter.drawPixmap(m_menuBtnRect, mb);
    }
    else if (m_state == GameState::Paused)
    {
        // 暂停:游戏画面 + 半透明蒙层 + 大菜单(墓碑)
        drawGame(&painter);

        painter.fillRect(rect(), QColor(0, 0, 0, 150));

        // 大菜单背景(屏幕中央)
        const int bigW = m_bigMenuBg.width();
        const int bigH = m_bigMenuBg.height();
        QRect bigMenuRect((WIDTH - bigW) / 2, (HEIGHT - bigH) / 2, bigW, bigH);
        if (!m_bigMenuBg.isNull())
            painter.drawPixmap(bigMenuRect, m_bigMenuBg);
        else
        {
            painter.setBrush(QColor(40, 40, 40, 240));
            painter.drawRoundedRect(bigMenuRect, 12, 12);
        }

        // ============ 速度档位:墓碑顶部,使用 < 中 > 形式 ============
        // 整体一行居中:左按钮 + 速度文字 + 右按钮
        QFont speedFont = painter.font();
        speedFont.setPointSize(15);
        speedFont.setBold(true);
        painter.setFont(speedFont);
        painter.setPen(QColor(240, 240, 240));

        // 速度档位汉字
        QString speedWord;
        switch (m_spawnSpeed)
        {
        case SpawnSpeed::Slow:   speedWord = "易"; break;
        case SpawnSpeed::Normal: speedWord = "中"; break;
        case SpawnSpeed::Fast:   speedWord = "难"; break;
        }
        const int speedRowY = bigMenuRect.top() + 180;
        const int obW = m_optionBtn.width();
        const int obH = m_optionBtn.height();

        // 左箭头按钮(<)
        m_speedDownRect = QRect(bigMenuRect.left() + 110, speedRowY, obW, obH);
        QPixmap& obDown = m_hoveredSpeedDown ? m_optionBtnHover : m_optionBtn;
        if (!obDown.isNull())
            painter.drawPixmap(m_speedDownRect, obDown);
        // 在左按钮上叠一个左箭头"◀"字符(更明显)
        painter.setPen(QColor(255, 200, 50));
        QFont arrowFont = painter.font();
        arrowFont.setPointSize(18);
        arrowFont.setBold(true);
        painter.setFont(arrowFont);
        painter.drawText(m_speedDownRect, Qt::AlignCenter, "◀");

        speedFont.setPointSize(16);
        painter.setFont(speedFont);
        painter.setPen(QColor(255, 220, 100));
        //QString speedLabel = QString("%1  (%2 帧)").arg(speedWord)
        //                                         .arg(static_cast<int>(m_spawnSpeed));
        QString speedLabel = QString("  难度：     %1").arg(speedWord);

        QRect speedLabelRect(m_speedDownRect.right() + 10, speedRowY, 160, obH);

        painter.drawText(speedLabelRect, Qt::AlignVCenter | Qt::AlignLeft, speedLabel);

        // 右箭头按钮(>)
        m_speedUpRect = QRect(speedLabelRect.right() + 10, speedRowY, obW, obH);
        QPixmap& obUp = m_hoveredSpeedUp ? m_optionBtnHover : m_optionBtn;
        if (!obUp.isNull())
            painter.drawPixmap(m_speedUpRect, obUp);
        painter.setFont(arrowFont);
        painter.setPen(QColor(255, 200, 50));
        painter.drawText(m_speedUpRect, Qt::AlignCenter, "▶");

        // ============ "返回游戏"按钮:放在"重新开始"按钮上方 ============
        // (放在墓碑底部方框内,比"重新开始"高一些,符合原版 PvZ 暂停菜单布局)
        const int retW = m_returnBtn.width();
        const int retH = m_returnBtn.height();
        m_returnBtnRect = QRect(bigMenuRect.left() + (bigW - retW) / 2,
                                bigMenuRect.bottom() - retH - 65,
                                retW, retH);
        QPixmap& retBtn = m_hoveredReturn ? m_returnBtnHover : m_returnBtn;
        if (!retBtn.isNull())
            painter.drawPixmap(m_returnBtnRect, retBtn);

        // ============ "重新开始"按钮:放在墓碑底部方框里 ============
        const int rbW = m_restartBtn.width();
        const int rbH = m_restartBtn.height();
        // 居中放在墓碑底部方框内
        m_restartBtnRect = QRect(bigMenuRect.left() + (bigW - rbW) / 2,
                                  bigMenuRect.bottom() - rbH - 200,
                                  rbW, rbH);
        QPixmap& rb = m_hoveredRestart ? m_restartBtnHover : m_restartBtn;
        if (!rb.isNull())
            painter.drawPixmap(m_restartBtnRect, rb);
    }
}

void Game::closeEvent(QCloseEvent *ev)
{
    cleanGame();
}

// ============== 鼠标交互 ==============

void Game::mousePressEvent(QMouseEvent *ev)
{
    const QPoint p = ev->pos();

    if (m_state == GameState::Menu)
    {
        if (m_startBtnRect.contains(p))
        {
            playSound(soundAssetPath("buttonclick.ogg"));
            onClickStart();
            update();
        }
    }
    else if (m_state == GameState::Playing)
    {
        // 点计分板或菜单按钮 → 暂停
        if (m_menuBtnRect.contains(p) || m_scorePanelRect.contains(p))
        {
            playSound(soundAssetPath("buttonclick.ogg"));
            onClickMenu();
            update();
        }
    }
    else if (m_state == GameState::Paused)
    {
        // 注意:判断顺序——先判断在"返回游戏"按钮,再判断"重新开始"按钮,
        // 避免两个按钮的 hit-rect 在垂直方向上重叠时点错
        if (m_returnBtnRect.contains(p))
        {
            playSound(soundAssetPath("buttonclick.ogg"));
            onClickReturn();
            update();
        }
        else if (m_restartBtnRect.contains(p))
        {
            playSound(soundAssetPath("buttonclick.ogg"));
            onClickRestart();
            update();
        }
        else if (m_speedDownRect.contains(p))
        {
            playSound(soundAssetPath("buttonclick.ogg"));
            onClickSpeedDown();
            update();
        }
        else if (m_speedUpRect.contains(p))
        {
            playSound(soundAssetPath("buttonclick.ogg"));
            onClickSpeedUp();
            update();
        }
    }
}

void Game::mouseMoveEvent(QMouseEvent *ev)
{
    // 鼠标悬停高亮(简化版:只切换 hover 标志,Qt 重绘由 update() 触发)
    const QPoint p = ev->pos();
    bool needUpdate = false;

    bool newHoverStart      = (m_state == GameState::Menu && m_startBtnRect.contains(p));
    bool newHoverMenu       = (m_state == GameState::Playing && m_menuBtnRect.contains(p));
    bool newHoverRestart    = (m_state == GameState::Paused && m_restartBtnRect.contains(p));
    bool newHoverReturn     = (m_state == GameState::Paused && m_returnBtnRect.contains(p));
    bool newHoverSpeedDown  = (m_state == GameState::Paused && m_speedDownRect.contains(p));
    bool newHoverSpeedUp    = (m_state == GameState::Paused && m_speedUpRect.contains(p));

    if (newHoverStart      != m_hoveredStart)      { m_hoveredStart      = newHoverStart;      needUpdate = true; }
    if (newHoverMenu       != m_hoveredMenu)       { m_hoveredMenu       = newHoverMenu;       needUpdate = true; }
    if (newHoverRestart    != m_hoveredRestart)    { m_hoveredRestart    = newHoverRestart;    needUpdate = true; }
    if (newHoverReturn     != m_hoveredReturn)     { m_hoveredReturn     = newHoverReturn;     needUpdate = true; }
    if (newHoverSpeedDown  != m_hoveredSpeedDown)  { m_hoveredSpeedDown  = newHoverSpeedDown;  needUpdate = true; }
    if (newHoverSpeedUp    != m_hoveredSpeedUp)    { m_hoveredSpeedUp    = newHoverSpeedUp;    needUpdate = true; }

    if (needUpdate) update();
}

// ============== 按钮业务逻辑 ==============

void Game::onClickStart()
{
    startNewGame();
}

void Game::onClickMenu()
{
    pauseGame();
}

void Game::onClickRestart()
{
    // "重新游戏"= 回到主菜单(让玩家再次按开始)
    backToMainMenu();
}

void Game::onClickReturn()
{
    // "返回游戏"= 继续当前游戏(从 Paused 回到 Playing)
    // 与 ESC 键、原始 resumeGame() 行为一致
    resumeGame();
}

void Game::onClickSpeedDown()
{
    speedDown();
}

void Game::onClickSpeedUp()
{
    speedUp();
}

void Game::startNewGame()
{
    m_state = GameState::Playing;
    m_score = 0;
    m_spawnSpeed = SpawnSpeed::Normal;

    // 清理现有僵尸(让玩家重新开始时地图干净)
    for (auto e : Mgr->getEntityByType<Zombies>(EntityType::ET_Zombies))
        e->destroy();

    // 把玩家放回初始位置
    player->setPos(250, 300);
    player->velocity = QVector2D(0, 0);
    player->runAnimation("normal");

    // 启动 BGM(按一次 StartButton 播一次,跨状态切换 BGM 不掉)
    SoundMgr->playBgm(SoundType::BGM);
    // 按下 StartButton 之后播一次"僵尸来了"提示音
    SoundMgr->playSfx(SoundType::ZombieComing);
}

void Game::pauseGame()
{
    if (m_state != GameState::Playing) return;
    m_state = GameState::Paused;
    // 玩家停止移动
    player->velocity = QVector2D(0, 0);
}

void Game::resumeGame()
{
    if (m_state != GameState::Paused) return;
    m_state = GameState::Playing;
}

void Game::backToMainMenu()
{
    // 清空僵尸 + 重置玩家位置 + 分数清零 + 进 Menu 状态
    for (auto e : Mgr->getEntityByType<Zombies>(EntityType::ET_Zombies))
        e->destroy();
    m_score = 0;
    m_spawnSpeed = SpawnSpeed::Normal;
    player->setPos(250, 300);
    player->velocity = QVector2D(0, 0);
    player->runAnimation("normal");
    m_state = GameState::Menu;
}

void Game::speedUp()
{
    // 速度档位:慢(180) -> 中(120) -> 快(60)
    switch (m_spawnSpeed)
    {
    case SpawnSpeed::Slow:   m_spawnSpeed = SpawnSpeed::Normal; break;
    case SpawnSpeed::Normal: m_spawnSpeed = SpawnSpeed::Fast;   break;
    case SpawnSpeed::Fast:   /* 已是最快,保持 */                   break;
    }
}

void Game::speedDown()
{
    switch (m_spawnSpeed)
    {
    case SpawnSpeed::Fast:   m_spawnSpeed = SpawnSpeed::Normal; break;
    case SpawnSpeed::Normal: m_spawnSpeed = SpawnSpeed::Slow;   break;
    case SpawnSpeed::Slow:   /* 已是最慢,保持 */                   break;
    }
}

void Game::keyPressEvent(QKeyEvent *ev)
{
    // ESC: 在 Playing 和 Paused 之间切换
    if (ev->key() == Qt::Key_Escape)
    {
        if (m_state == GameState::Playing)
        {
            pauseGame();
            update();
            return;
        }
        else if (m_state == GameState::Paused)
        {
            resumeGame();
            update();
            return;
        }
    }

    // 菜单状态下,所有按键忽略
    if (m_state != GameState::Playing) return;

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
    // 菜单/暂停状态下不响应移动键释放
    if (m_state != GameState::Playing) return;

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

void Game::initSound()
{
    // 初始化 SoundManager 并循环播放战斗 BGM
    SoundMgr->init();
    SoundMgr->playBgm(SoundType::BGM);
    // 不在这里播 ZombieComing —— 改成按 StartButton 之后由 startNewGame() 播一次
}

void Game::playSound(const QString &path)
{
    // legacy 接口,目前没有调用方;保留以兼容 Game.h 的旧声明
    Q_UNUSED(path);
}

void Game::playSound(SoundType type)
{
    SoundMgr->playSfx(type);
}











