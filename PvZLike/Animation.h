#ifndef ANIMATION_H
#define ANIMATION_H

#include <QList>
#include <QPixmap>
#include <QPainter>
#include <QVector2D>
class Animation
{
public:
    Animation(int animSpeed = 60);
    //添加帧贴图
    void addFrame(const QString& filename);
    QPixmap getFrame(int index)const;
    qsizetype count()const;
    void setPos(const QVector2D& pos);

    void update();
    void draw(QPainter*painter);
    inline bool finished()const{return index == count() - 1;}

private:
    QList<QPixmap> pixmaps;
    int xPos = 2000;
    int yPos = 2000;
    int index = 0;
    int animSpeed = 60;
};

#endif // ANIMATION_H
