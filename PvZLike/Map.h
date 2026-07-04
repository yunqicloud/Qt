#ifndef MAP_H
#define MAP_H
#include "Entity.h"
class Map : public Entity
{
public:
    Map();
    void update()override;
    void draw(QPainter*painter)override;
private:
    QPixmap pixmap;
    int pos;
};

#endif // MAP_H
