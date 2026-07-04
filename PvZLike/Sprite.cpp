#include "Sprite.h"
#include "Game.h"

Sprite::Sprite() {}

Sprite::Sprite(int x, int y, const QString &filename)
    :position(x, y)
{
    pixmap.load(filename);
}

void Sprite::update()
{
    position += speed * velocity;
    collider = QRect(position.toPoint(), pixmap.size());


}

void Sprite::draw(QPainter *painter)
{
    painter -> drawPixmap(position.toPoint(),pixmap);
}

void Sprite::setPos(int x, int y)
{
    position.setX(x);
    position.setY(y);
}

void Sprite::moveBy(int dx, int dy)
{
    position += QVector2D(dx,dy);
}


