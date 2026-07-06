#ifndef SPRITE_H
#define SPRITE_H
#include "Entity.h"
#include <QVector2D>
#include "Animation.h"
#include <QMap>
class Sprite : public Entity
{
public:
    Sprite();
    Sprite(int x,int y, const QString& filename);
    ~Sprite();
    void update()override;
    void draw(QPainter* painter)override;

    void setPos(int x,int y);
    void moveBy(int dx, int dy);
    inline int width()const {return pixmap.width();};
    inline int height()const {return pixmap.height();};

    Animation* addAnimation(const QString& tag,Animation* anim);
    void runAnimation(const QString& tag,bool autoDel = false);

    QString getCurrAnimationName() const;
public:
    QVector2D position;
    QPixmap pixmap;

    QVector2D velocity;
    int speed = 3;
    //碰撞器
    QRect collider;



private:
    QMap<QString,Animation*> anims;
    Animation* currentAnim = nullptr;
    QString curAnimName;
    bool autoDel = false;
};

#endif // SPRITE_H
