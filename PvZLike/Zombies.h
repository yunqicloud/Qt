#ifndef ZOMBIES_H
#define ZOMBIES_H

#include "Sprite.h"

class Zombies : public Sprite
{
public:
    using Sprite::Sprite;
    void update()override;
};

#endif // ZOMBIES_H
