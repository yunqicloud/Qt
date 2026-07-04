#include "Plants.h"
#include "Manager.h"
#include "Bullet.h"



void Plants::shoot()
{
    //添加子弹
    auto b = new Bullet(position.x(), position.y(), "PeaNormal_0.png");
    b -> setPos(position.x() + 25, position.y());
    b -> speed += 3;
    b -> velocity.setX(1);
    Mgr -> addEntity(b);
}
