#include "Plants.h"
#include "Manager.h"
#include "Bullet.h"
#include "Global.h"
#include "SoundManager.h"



void Plants::shoot()
{
    //添加子弹
    auto b = new Bullet(position.x(), position.y(), "graphics/Bullets/PeaNormal/PeaNormal_0.png");
    b -> setPos(position.x() + 25, position.y());
    b -> speed += 3;
    b -> velocity.setX(1);
    b -> setType(EntityType::ET_Bullet);
    Mgr -> addEntity(b);
    Animation* explode = b->addAnimation("explode", new Animation(120));
    explode -> addFrame("graphics/Bullets/PeaNormal/PeaNormalExplode_0.png");

    // 发射音效
    SoundMgr->playSfx(SoundType::Shoot);
}
