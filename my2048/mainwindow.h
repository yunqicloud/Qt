#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qpushbutton>
#include <QTime>
#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QRandomGenerator>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *event) override;

    QPushButton *button;
    int s[4][4];
    int score = 0;
    bool state;

    void PressUp();
    void PressDown();
    void PressLeft();
    void PressRight();

    //随机生成函数 判断结束函数
    void myRand();

    struct Ns{
        int i;
        int j;
    };

public slots://槽函数？
        void slotStart();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
