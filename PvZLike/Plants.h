#ifndef PLANTS_H
#define PLANTS_H
#include "Sprite.h"


class Plants : public Sprite
{
public:
    using Sprite::Sprite;
    void shoot();
};

#endif // PLANTS_H
