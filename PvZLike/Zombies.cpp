#include "Zombies.h"


void Zombies::update()
{
    if(position.x() < -200)
    {
        destroy();
    }
    Sprite::update();
}
