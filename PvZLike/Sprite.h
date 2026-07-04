#ifndef SPRITE_H
#define SPRITE_H
#include "Entity.h"
#include <QVector2D>
class Sprite : public Entity
{
public:
    Sprite();
    Sprite(int x,int y, const QString& filename);
    void update()override;
    void draw(QPainter* painter)override;

    void setPos(int x,int y);
    void moveBy(int dx, int dy);
    inline int width()const {return pixmap.width();};
    inline int height()const {return pixmap.height();};

public:
    QVector2D position;
    QPixmap pixmap;

    QVector2D velocity;
    int speed = 3;
    //碰撞器
    QRect collider;
};

#endif // SPRITE_H
