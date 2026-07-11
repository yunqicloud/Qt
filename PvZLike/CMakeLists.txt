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

# 把项目内的 R/ 资源目录在编译时以绝对路径注入代码。
# 代码里的 PVL_R_DIR 直接展开成源码 R/ 路径,
# 加载图片/音频走源码目录,不依赖运行时拷贝。
target_compile_definitions(PvZLike PRIVATE
    PVL_R_DIR="${CMAKE_CURRENT_SOURCE_DIR}/R"
)

# 同时也在 build 时把 R/ 拷贝到 exe 旁(双保险):
# - 开发时 main.cpp 启动看到 PVL_R_DIR 指向源码 R/,直接读
# - 分发时如果拿不到源码路径,exe 旁的 R/ 也能跑
add_custom_command(TARGET PvZLike POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/R"
        "$<TARGET_FILE_DIR:PvZLike>/R"
)

install(TARGETS PvZLike
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBIR}
)