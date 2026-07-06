#include "Animation.h"
#include "Game.h"
Animation::Animation(int animSpeed) : animSpeed(animSpeed)
{

}

void Animation::addFrame(const QString &filename)
{
    return pixmaps.append(QPixmap(filename));
}

QPixmap Animation::getFrame(int index) const
{
    return pixmaps[index];
}

qsizetype Animation::count() const
{
    return pixmaps.count();
}

void Animation::setPos(const QVector2D &pos)
{
    xPos = pos.x();
    yPos = pos.y();
}

void Animation::update()
{
    index = Game::globalElapsedTimer.elapsed() / animSpeed % count();
}

void Animation::draw(QPainter *painter)
{
    painter -> drawPixmap(xPos,yPos,pixmaps[index]);
}

