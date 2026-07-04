#include "Bullet.h"

void Bullet::update()
{
    if(position.x() > 1400)
    {
        destroy();
    }
    Sprite::update();
}
