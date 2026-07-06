cmake_minimum_required(VERSION 3.19)
project(PvZLike LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Multimedia)

qt_standard_project_setup()

# 让 CMake 自动发现并编译 .qrc 资源文件(生成 qrc_*.cpp 并链接进目标)
set(CMAKE_AUTORCC ON)

qt_add_executable(PvZLike
    main.cpp
    Game.h Game.cpp
    Entity.h
    Sprite.h Sprite.cpp
    Manager.h

    Map.h Map.cpp
    Resource.qrc
    Plants.h Plants.cpp
    Bullet.h Bullet.cpp
    Zombies.h Zombies.cpp
    Global.h
    Animation.h Animation.cpp
    SoundManager.h SoundManager.cpp

)

target_link_libraries(PvZLike
    PRIVATE
        Qt::Widgets
        Qt::Multimedia
)

install(TARGETS PvZLike
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
