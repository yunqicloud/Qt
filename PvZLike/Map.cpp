#include "Map.h"
#include "Game.h"

// 项目内资源根目录
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

Map::Map()
{
    pos = 0;
    pixmap.load(rRoot() + "/graphics/Background/Background_0.jpg");
}

void Map::update()
{

}

void Map::draw(QPainter *painter)
{
    painter -> drawPixmap(0,pos,pixmap);
}
