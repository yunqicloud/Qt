#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QObject>
#include <QVector>

class GameBoard : public QObject
{
    Q_OBJECT
public:
    enum Direction { Up, Down, Left, Right };
    Q_ENUM(Direction)

    explicit GameBoard(QObject *parent = nullptr);
    void reset();

    // Returns true if the move changed the board.
    bool move(Direction dir);

    int  score()    const { return m_score; }
    int  bestScore() const { return m_best; }
    void setBestScore(int v);

    int  tile(int r, int c) const { return m_grid[r][c]; }
    int  rows() const { return 4; }
    int  cols() const { return 4; }

    bool canMove() const;          // true if any move possible
    bool hasWon()  const { return m_won; }
    bool isOver()  const;

signals:
    void boardChanged();           // tiles changed (visual update needed)
    void scoreChanged(int score, int best);
    void gameOver();
    void won();

private:
    void spawnTile();              // spawn 2 (90%) or 4 (10%)
    void recomputeAvailable();

    QVector<QVector<int>> m_grid;  // 4x4
    QVector<int>          m_empty; // indices of empty cells (linear: r*4+c)
    int  m_score   = 0;
    int  m_best    = 0;
    bool m_won     = false;        // latched: keep showing "you won"
};

#endif // GAMEBOARD_H