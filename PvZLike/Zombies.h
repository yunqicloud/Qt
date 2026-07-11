#ifndef ZOMBIES_H
#define ZOMBIES_H

#include "Sprite.h"

class Zombies : public Sprite
{
public:
    using Sprite::Sprite;
    void update()override;

    // 死亡动画中进行中标志。一旦为 true 就跳过碰撞测试 + 玩家互动,
    // 避免 700ms 期间被反复触发 die + 堆积 QTimer::singleShot 队列
    // (这会导致 "音效重复" + "僵尸不释放")
    bool dying = false;
};

#endif // ZOMBIES_H
