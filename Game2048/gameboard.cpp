#include "gameboard.h"
#include <QRandomGenerator>
#include <algorithm>
#include <QSet>

GameBoard::GameBoard(QObject *parent) : QObject(parent)
{
    m_grid.resize(4);
    for (int i = 0; i < 4; ++i) m_grid[i].resize(4);

    reset();
}

void GameBoard::reset()
{
    for (auto &row : m_grid) std::fill(row.begin(), row.end(), 0);
    m_score = 0;
    m_won   = false;
    recomputeAvailable();
    spawnTile();
    spawnTile();
    emit boardChanged();
    emit scoreChanged(m_score, m_best);
}

bool GameBoard::move(Direction dir)
{
    // Capture pre-move state so we can detect "no change".
    auto before = m_grid;

    auto processLine = [&](QVector<int> &line, int &gain) {
        // 1. compress (slide non-zeros to the front, in the move direction)
        QVector<int> a;
        for (int v : line) if (v != 0) a.push_back(v);
        // 2. merge adjacent equals (left-to-right only, never merge twice in one move)
        QVector<int> b;
        b.reserve(4);
        bool merged[4] = {false, false, false, false};
        for (int i = 0; i < a.size(); ++i) {
            if (i + 1 < a.size() && a[i] == a[i + 1] && !merged[i] && !merged[i + 1]) {
                int nv = a[i] * 2;
                b.push_back(nv);
                gain += nv;
                if (nv == 2048 && !m_won) {
                    m_won = true;
                    emit won();
                }
                merged[i] = true;
                merged[i + 1] = true;
                ++i; // skip the next element we already merged with
            } else {
                b.push_back(a[i]);
            }
        }
        while (b.size() < 4) b.push_back(0);
        line = b;
    };

    int gain = 0;
    switch (dir) {
    case Left: {
        for (int r = 0; r < 4; ++r) processLine(m_grid[r], gain);
        break;
    }
    case Right: {
        for (int r = 0; r < 4; ++r) {
            QVector<int> rev = m_grid[r];
            std::reverse(rev.begin(), rev.end());
            processLine(rev, gain);
            std::reverse(rev.begin(), rev.end());
            m_grid[r] = rev;
        }
        break;
    }
    case Up: {
        for (int c = 0; c < 4; ++c) {
            QVector<int> col;
            for (int r = 0; r < 4; ++r) col.push_back(m_grid[r][c]);
            processLine(col, gain);
            for (int r = 0; r < 4; ++r) m_grid[r][c] = col[r];
        }
        break;
    }
    case Down: {
        for (int c = 0; c < 4; ++c) {
            QVector<int> col;
            for (int r = 0; r < 4; ++r) col.push_back(m_grid[r][c]);
            std::reverse(col.begin(), col.end());
            processLine(col, gain);
            std::reverse(col.begin(), col.end());
            for (int r = 0; r < 4; ++r) m_grid[r][c] = col[r];
        }
        break;
    }
    }

    bool changed = (before != m_grid);
    if (!changed) return false;

    m_score += gain;
    if (m_score > m_best) m_best = m_score;

    spawnTile();
    recomputeAvailable();
    emit boardChanged();
    emit scoreChanged(m_score, m_best);

    if (isOver()) emit gameOver();
    return true;
}

void GameBoard::spawnTile()
{
    recomputeAvailable();
    if (m_empty.isEmpty()) return;

    int idx = m_empty[QRandomGenerator::global()->bounded(m_empty.size())];
    int r = idx / 4, c = idx % 4;
    m_grid[r][c] = (QRandomGenerator::global()->bounded(10) == 0) ? 4 : 2;
}

void GameBoard::recomputeAvailable()
{
    m_empty.clear();
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (m_grid[r][c] == 0) m_empty.push_back(r * 4 + c);
}

bool GameBoard::canMove() const
{
    if (!m_empty.isEmpty()) return true;
    // Any horizontal or vertical neighbor equal?
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (r + 1 < 4 && m_grid[r][c] == m_grid[r + 1][c]) return true;
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (c + 1 < 4 && m_grid[r][c] == m_grid[r][c + 1]) return true;
    return false;
}

bool GameBoard::isOver() const
{
    return m_empty.isEmpty() && !canMove();
}

void GameBoard::setBestScore(int v)
{
    if (v > m_best) {
        m_best = v;
        emit scoreChanged(m_score, m_best);
    } else {
        m_best = v;
    }
}