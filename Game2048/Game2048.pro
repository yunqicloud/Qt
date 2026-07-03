QT       += core gui widgets

CONFIG   += c++17
TARGET    = Game2048
TEMPLATE  = app

DEFINES  += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    gameboard.cpp

HEADERS += \
    mainwindow.h \
    gameboard.h