#include "Sprite.h"
#include "Game.h"

Sprite::Sprite() {}

Sprite::Sprite(int x, int y, const QString &filename)
    :position(x, y)
{
    pixmap.load(filename);
}

Sprite::~Sprite()
{
    for(auto anims : anims)
    {
        delete anims;
    }
}

void Sprite::update()
{
    position += speed * velocity;
    collider = QRect(position.toPoint(), pixmap.size());
    if(currentAnim)
    {
        currentAnim -> update();
        currentAnim -> setPos(position);
        if(autoDel && currentAnim -> finished())
        {
            destroy();
        }
    }

}

void Sprite::draw(QPainter *painter)
{
    if(!currentAnim)
        painter -> drawPixmap(position.toPoint(),pixmap);
    else
        currentAnim -> draw(painter);
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

Animation *Sprite::addAnimation(const QString &tag, Animation *anim)
{
    if(!anim)
    {
        qWarning()<<"null";
        return nullptr;
    }
    if (anims.contains(tag))
    {
        qWarning()<<"已存在" + tag + "动画";
        return anim;
    }
    anims.insert(tag, anim);
    return anim;
}

void Sprite::runAnimation(const QString &tag,bool autoDel)
{
    this -> autoDel = autoDel;
    if (!anims.contains(tag))
    {
        qWarning()<<"不存在" + tag + "动画";
        return;
    }
    currentAnim = anims.value(tag);
}

QString Sprite::getCurrAnimationName() const
{
    return curAnimName;
}





