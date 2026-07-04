#ifndef MANAGER_H
#define MANAGER_H

#include "Entity.h"
#include <QSet>

#define Mgr Manager::getInstance()

class Manager
{
public:
    //获取管理的单例
    static Manager* getInstance()
    {
        static Manager* ins = nullptr;
        if(!ins)
        {
            ins = new Manager;
        }
        return ins;
    }

    template<typename T>
    T* addEntity(T* e)
    {
        entities.insert(e);
        return e;
    }

    qsizetype count()const {return entities.count();}

    void update()
    {
        for(auto& e : std::as_const(entities))
        {
            e -> update();
        }
    }

    void draw(QPainter*painter)
    {
        for (auto& e : std::as_const(entities))
        {
            e -> draw(painter);
        }
    }

    void refresh()
    {
        entities.removeIf([](Entity*e)
        {
            if(!e -> isActive())
            {
                delete e;
                return true;
            }
            return false;
        });
    }
private:
    QSet<Entity*> entities;
    Manager(){};
};



#endif // MANAGER_H
