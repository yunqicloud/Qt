#include <QCoreApplication>
#include <QApplication>
#include "Game.h"

// 直接 extern qrc_Resource.cpp 生成的初始化函数,绕开 Q_INIT_RESOURCE 宏
// (后者在某些编译选项下会被优化掉调用,资源就注册不上了)
// 这个函数把 rcc 嵌入的 qrc_Resource.cpp.obj 里的资源数据注册到全局资源树,
// 之后 QFile::open("qrc:/...") 才能解析。
extern int qInitResources_Resource();

int main(int argc, char *argv[])
{
    // 必须放在 QApplication 构造之前。返回值赋给 volatile 变量防止编译器
    // 把这个"看起来无副作用"的调用给优化掉(GCC 在 -O2 下会这么做)
    volatile int rc = qInitResources_Resource();
    (void)rc;

    //QCoreApplication a(argc, argv);
    QApplication a(argc, argv);

    Game game;
    game.initGame(1400, 600, "PvZ");
    game.run();

    return a.exec();
}