#include "Animation.h"
#include "Game.h"

// 项目内资源根目录(同 SoundManager/Game 用的 PVL_R_DIR 宏)
#include <QString>
namespace {
QString rRoot()
{
#ifdef PVL_R_DIR
    return QString(PVL_R_DIR);
#else
    return QString();
#endif
}
}

Animation::Animation(int animSpeed) : animSpeed(animSpeed)
{

}

void Animation::addFrame(const QString &relPath)
{
    // 参数是相对项目 R/ 的路径(如 "graphics/Plants/Peashooter/Peashooter_0.png")
    // 内部拼接成项目内绝对路径,完全脱离 qrc
    const QString full = rRoot() + "/" + relPath;
    pixmaps.append(QPixmap(full));
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

