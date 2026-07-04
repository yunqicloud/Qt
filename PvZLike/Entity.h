#ifndef ENTITY_H
#define ENTITY_H
#include <QPainter>
class Entity
{
public:
    Entity(){};
    virtual ~Entity(){};
    virtual void update() = 0;
    virtual void draw(QPainter* painter) = 0;

    inline bool isActive()const {return active;}
    inline void destroy() {active = false;}

    inline int type()const {return m_type;}
    inline void setType(int type){m_type = type;}
private:
    bool active = true;//实体是不是活的
    int m_type = 0;//实体类型
};



#endif // ENTITY_H
