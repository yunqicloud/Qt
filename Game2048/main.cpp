#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("2048");
    QApplication::setOrganizationName("Mavis");

    MainWindow w;
    w.show();
    return app.exec();
}