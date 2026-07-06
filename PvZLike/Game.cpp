#include "Game.h"
#include <QIcon>
#include <QPainter>
#include <QTimer>
#include <QDateTime>
#include "Plants.h"
#include <QKeyEvent>
#include "Map.h"
#include "Zombies.h"
#include <qrandom.h>
#include "Global.h"
#include "bullet.h"
#include "SoundManager.h"



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
    globalElapsedTimer.start();

    map = new Map;
    player = Mgr -> addEntity(new Plants(0,0,"Peashooter_0.png"));
    player -> setType(EntityType::ET_Player);

    auto plantsAnim = player -> addAnimation("normal",new Animation);
    plantsAnim -> addFrame("Peashooter_0.png");
    plantsAnim -> addFrame("Peashooter_1.png");
    plantsAnim -> addFrame("Peashooter_2.png");
    plantsAnim -> addFrame("Peashooter_3.png");
    plantsAnim -> addFrame("Peashooter_4.png");
    plantsAnim -> addFrame("Peashooter_5.png");
    plantsAnim -> addFrame("Peashooter_6.png");
    plantsAnim -> addFrame("Peashooter_7.png");
    plantsAnim -> addFrame("Peashooter_8.png");
    plantsAnim -> addFrame("Peashooter_9.png");
    plantsAnim -> addFrame("Peashooter_10.png");
    plantsAnim -> addFrame("Peashooter_11.png");
    plantsAnim -> addFrame("Peashooter_12.png");
    player -> runAnimation("normal");

    // 启动音效系统并循环播放战斗 BGM
    initSound();

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
    if(i % 45 == 0)
    {
        //player -> shoot();
    }

    //僵尸出场
    if(i > 0 && i % 120 == 0)
    {
        auto e = Mgr -> addEntity(new Zombies(2000,2000,"Zombie_0.png"));
        int randY = QRandomGenerator::global()->bounded(Game::HEIGHT - e->height());
        e -> setPos(1450,randY);
        e -> speed = 1;
        e -> velocity.setX(-1);
        e -> setType(EntityType::ET_Zombies);
        Animation* walkAnim = e->addAnimation("normal", new Animation);
        walkAnim -> addFrame("Zombie_0.png");
        walkAnim -> addFrame("Zombie_1.png");
        walkAnim -> addFrame("Zombie_2.png");
        walkAnim -> addFrame("Zombie_3.png");
        walkAnim -> addFrame("Zombie_4.png");
        walkAnim -> addFrame("Zombie_5.png");
        walkAnim -> addFrame("Zombie_6.png");
        walkAnim -> addFrame("Zombie_7.png");
        walkAnim -> addFrame("Zombie_8.png");
        walkAnim -> addFrame("Zombie_9.png");
        walkAnim -> addFrame("Zombie_10.png");
        walkAnim -> addFrame("Zombie_11.png");
        walkAnim -> addFrame("Zombie_12.png");
        walkAnim -> addFrame("Zombie_13.png");
        walkAnim -> addFrame("Zombie_14.png");
        walkAnim -> addFrame("Zombie_15.png");
        walkAnim -> addFrame("Zombie_16.png");
        walkAnim -> addFrame("Zombie_17.png");
        walkAnim -> addFrame("Zombie_18.png");
        walkAnim -> addFrame("Zombie_19.png");
        walkAnim -> addFrame("Zombie_20.png");
        walkAnim -> addFrame("Zombie_21.png");

        e->runAnimation("normal");

        Animation* dieAnim = e->addAnimation("die", new Animation(120));
        dieAnim -> addFrame("ZombieDie_0.png");
        dieAnim -> addFrame("ZombieDie_1.png");
        dieAnim -> addFrame("ZombieDie_2.png");
        dieAnim -> addFrame("ZombieDie_3.png");
        dieAnim -> addFrame("ZombieDie_4.png");
        dieAnim -> addFrame("ZombieDie_5.png");
        dieAnim -> addFrame("ZombieDie_6.png");
        dieAnim -> addFrame("ZombieDie_7.png");
        dieAnim -> addFrame("ZombieDie_8.png");
        dieAnim -> addFrame("ZombieDie_9.png");

        Animation* attackAnim = e->addAnimation("attack", new Animation(120));
        attackAnim -> addFrame("ZombieAttack_0.png");
        attackAnim -> addFrame("ZombieAttack_1.png");
        attackAnim -> addFrame("ZombieAttack_2.png");
        attackAnim -> addFrame("ZombieAttack_3.png");
        attackAnim -> addFrame("ZombieAttack_4.png");
        attackAnim -> addFrame("ZombieAttack_5.png");
        attackAnim -> addFrame("ZombieAttack_6.png");
        attackAnim -> addFrame("ZombieAttack_7.png");
        attackAnim -> addFrame("ZombieAttack_8.png");
        attackAnim -> addFrame("ZombieAttack_9.png");
        attackAnim -> addFrame("ZombieAttack_10.png");
        attackAnim -> addFrame("ZombieAttack_11.png");
        attackAnim -> addFrame("ZombieAttack_12.png");
        attackAnim -> addFrame("ZombieAttack_13.png");
        attackAnim -> addFrame("ZombieAttack_14.png");
        attackAnim -> addFrame("ZombieAttack_15.png");
        attackAnim -> addFrame("ZombieAttack_16.png");
        attackAnim -> addFrame("ZombieAttack_17.png");
        attackAnim -> addFrame("ZombieAttack_18.png");
        attackAnim -> addFrame("ZombieAttack_19.png");
        attackAnim -> addFrame("ZombieAttack_20.png");
    }

    //碰撞检测
    for(auto e : Mgr -> getEntityByType<Zombies>(EntityType::ET_Zombies))
    {
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
                QTimer::singleShot(700, [=](){
                    e->destroy();
                });
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

void Game::initSound()
{
    // 初始化 SoundManager 并循环播放战斗 BGM
    SoundMgr->init();
    SoundMgr->playBgm(SoundType::BGM);
    // 进游戏时播一次"僵尸来了"提示音(后续每波僵尸不再单独播)
    SoundMgr->playSfx(SoundType::ZombieComing);
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











