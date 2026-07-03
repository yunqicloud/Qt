cmake_minimum_required(VERSION 3.19)
project(PvZLike LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(PvZLike
    main.cpp
)

target_link_libraries(PvZLike
    PRIVATE
        Qt::Widgets
)

install(TARGETS PvZLike
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
