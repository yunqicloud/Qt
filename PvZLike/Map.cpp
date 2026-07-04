#include "Map.h"
#include "Game.h"

Map::Map()
{
    pos = 0;
    pixmap.load("Background_0.jpg");
}

void Map::update()
{

}

void Map::draw(QPainter *painter)
{
    painter -> drawPixmap(0,pos,pixmap);
}
