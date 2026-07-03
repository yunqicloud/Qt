#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gameboard.h"

class QLabel;
class QPushButton;
class BoardView;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void closeEvent(QCloseEvent *e) override;

private slots:
    void onRestart();
    void onBoardChanged();
    void onScoreChanged(int score, int best);
    void onGameOver();
    void onWon();

private:
    void loadBest();
    void saveBest();
    void showOverlay(const QString &line1, const QString &line2 = QString());
    void hideOverlay();

    GameBoard  *m_board;
    BoardView  *m_view;
    QLabel     *m_scoreLbl;
    QLabel     *m_bestLbl;
    QPushButton *m_restartBtn;
    QWidget    *m_overlay = nullptr;
};

class BoardView : public QWidget
{
    Q_OBJECT
public:
    explicit BoardView(GameBoard *board, QWidget *parent = nullptr);
    QSize sizeHint() const override { return QSize(460, 460); }

protected:
    void paintEvent(QPaintEvent *) override;

private:
    GameBoard *m_board;
};

#endif // MAINWINDOW_H