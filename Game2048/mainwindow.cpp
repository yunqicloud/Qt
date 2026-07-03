#include "mainwindow.h"

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#include <QFont>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>

namespace {
QColor tileBg(int v) {
    switch (v) {
    case 2:    return QColor("#eee4da");
    case 4:    return QColor("#ede0c8");
    case 8:    return QColor("#f2b179");
    case 16:   return QColor("#f59563");
    case 32:   return QColor("#f67c5f");
    case 64:   return QColor("#f65e3b");
    case 128:  return QColor("#edcf72");
    case 256:  return QColor("#edcc61");
    case 512:  return QColor("#edc850");
    case 1024: return QColor("#edc53f");
    case 2048: return QColor("#edc22e");
    default:   return QColor("#3c3a32"); // >2048
    }
}
QColor tileFg(int v) {
    return (v <= 4) ? QColor("#776e65") : QColor("#f9f6f2");
}
int tileFontPx(int v) {
    if (v < 100)   return 44;
    if (v < 1000)  return 38;
    if (v < 10000) return 30;
    return 24;
}
} // namespace

BoardView::BoardView(GameBoard *board, QWidget *parent)
    : QWidget(parent), m_board(board)
{
    setMinimumSize(420, 420);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(false);
}

void BoardView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    const int padding = 14;
    const int gap     = 12;
    const int W = width();
    const int H = height();
    const int side = qMin(W, H);
    const int x0 = (W - side) / 2;
    const int y0 = (H - side) / 2;

    // outer board background
    p.fillRect(rect(), QColor("#bbada0"));
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#bbada0"));
    p.drawRoundedRect(QRect(x0, y0, side, side), 8, 8);

    const int cell = (side - 2 * padding - 3 * gap) / 4;
    auto cellRect = [&](int r, int c) -> QRect {
        int x = x0 + padding + c * (cell + gap);
        int y = y0 + padding + r * (cell + gap);
        return QRect(x, y, cell, cell);
    };

    // empty cells background
    p.setBrush(QColor("#cdc1b4"));
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            p.drawRoundedRect(cellRect(r, c), 6, 6);

    // tiles
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            int v = m_board->tile(r, c);
            if (v == 0) continue;
            QRect rc = cellRect(r, c);
            p.setBrush(tileBg(v));
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(rc, 6, 6);

            QFont f = p.font();
            f.setBold(true);
            f.setPixelSize(tileFontPx(v));
            f.setFamily("Arial");
            p.setFont(f);
            p.setPen(tileFg(v));
            p.drawText(rc, Qt::AlignCenter, QString::number(v));
        }
    }
}

// ---------------- MainWindow ----------------

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_board(new GameBoard(this))
{
    setWindowTitle("2048");
    setFixedSize(520, 640);
    setStyleSheet(
        "QMainWindow { background: #faf8ef; }"
        "QLabel#title { color: #776e65; font: bold 44px 'Arial'; }"
        "QLabel#subtitle { color: #776e65; font: 14px 'Arial'; }"
        "QLabel#scoreLabel, QLabel#bestLabel { color: #bbada0; font: bold 11px 'Arial'; "
        "   text-transform: uppercase; letter-spacing: 1px; }"
        "QLabel#scoreValue, QLabel#bestValue { color: white; font: bold 22px 'Arial'; }"
        "QFrame#scoreBox, QFrame#bestBox { background: #bbada0; border-radius: 6px; }"
        "QPushButton { background: #8f7a66; color: #f9f6f2; border: none; "
        "   padding: 10px 22px; border-radius: 6px; font: bold 16px 'Arial'; }"
        "QPushButton:hover { background: #9f8b77; }"
        "QPushButton:pressed { background: #7a6655; }"
    );

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *root = new QVBoxLayout(central);
    root->setContentsMargins(20, 18, 20, 18);
    root->setSpacing(14);

    // Top: title + scores + restart
    QHBoxLayout *top = new QHBoxLayout();
    top->setSpacing(12);

    QVBoxLayout *titleCol = new QVBoxLayout();
    titleCol->setSpacing(0);
    QLabel *title = new QLabel("2048");
    title->setObjectName("title");
    QLabel *sub = new QLabel("Join the tiles, get to 2048!");
    sub->setObjectName("subtitle");
    titleCol->addWidget(title);
    titleCol->addWidget(sub);
    top->addLayout(titleCol);
    top->addStretch();

    auto makeScoreBox = [&](const QString &label, const QString &objVal, QLabel *&valLbl) {
        QFrame *box = new QFrame();
        box->setObjectName(label.toLower() + "Box");
        box->setFixedSize(96, 60);
        QVBoxLayout *v = new QVBoxLayout(box);
        v->setContentsMargins(0, 6, 0, 6);
        v->setSpacing(0);
        QLabel *l = new QLabel(label);
        l->setObjectName(label.toLower() + "Label");
        l->setAlignment(Qt::AlignCenter);
        valLbl = new QLabel("0");
        valLbl->setObjectName(label.toLower() + "Value");
        valLbl->setAlignment(Qt::AlignCenter);
        v->addWidget(l);
        v->addWidget(valLbl);
        return box;
    };
    QFrame *scoreBox = makeScoreBox("Score", "scoreValue", m_scoreLbl);
    QFrame *bestBox  = makeScoreBox("Best",  "bestValue",  m_bestLbl);
    top->addWidget(scoreBox);
    top->addWidget(bestBox);

    m_restartBtn = new QPushButton("New Game");
    m_restartBtn->setFixedHeight(60);
    top->addWidget(m_restartBtn);

    root->addLayout(top);

    // Board area
    m_view = new BoardView(m_board);
    QHBoxLayout *boardRow = new QHBoxLayout();
    boardRow->addStretch();
    boardRow->addWidget(m_view);
    boardRow->addStretch();
    root->addLayout(boardRow);

    // Footer hint
    QLabel *hint = new QLabel(
        "How to play: Use your <b>arrow keys</b> or <b>WASD</b> to move the tiles. "
        "Tiles with the same number merge into one when they touch. "
        "Add them up to reach <b>2048</b>!");
    hint->setWordWrap(true);
    hint->setStyleSheet("color:#776e65; font:13px 'Arial';");
    root->addWidget(hint);

    root->addStretch();

    // Wire up
    connect(m_board, &GameBoard::boardChanged,  this, &MainWindow::onBoardChanged);
    connect(m_board, &GameBoard::scoreChanged,  this, &MainWindow::onScoreChanged);
    connect(m_board, &GameBoard::gameOver,      this, &MainWindow::onGameOver);
    connect(m_board, &GameBoard::won,           this, &MainWindow::onWon);
    connect(m_restartBtn, &QPushButton::clicked, this, &MainWindow::onRestart);

    loadBest();
    onScoreChanged(0, m_board->bestScore());
    onBoardChanged(); // initial paint
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    GameBoard::Direction d;
    switch (e->key()) {
    case Qt::Key_Left:  case Qt::Key_A: d = GameBoard::Left;  break;
    case Qt::Key_Right: case Qt::Key_D: d = GameBoard::Right; break;
    case Qt::Key_Up:    case Qt::Key_W: d = GameBoard::Up;    break;
    case Qt::Key_Down:  case Qt::Key_S: d = GameBoard::Down;  break;
    default: return;
    }
    m_board->move(d);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    saveBest();
    QMainWindow::closeEvent(e);
}

void MainWindow::onRestart()
{
    saveBest();
    hideOverlay();
    m_board->reset();
}

void MainWindow::onBoardChanged()
{
    m_view->update();
}

void MainWindow::onScoreChanged(int score, int best)
{
    m_scoreLbl->setText(QString::number(score));
    m_bestLbl->setText(QString::number(best));
    if (score > m_board->bestScore()) m_board->setBestScore(score);
}

void MainWindow::onGameOver()
{
    saveBest();
    showOverlay("Game Over!", "Click New Game to try again");
}

void MainWindow::onWon()
{
    showOverlay("You Win!", "Keep going, or start a new game");
}

void MainWindow::loadBest()
{
    QSettings s(QSettings::IniFormat, QSettings::UserScope,
                QApplication::organizationName(), QApplication::applicationName());
    int b = s.value("best", 0).toInt();
    m_board->setBestScore(b);
}

void MainWindow::saveBest()
{
    QSettings s(QSettings::IniFormat, QSettings::UserScope,
                QApplication::organizationName(), QApplication::applicationName());
    s.setValue("best", m_board->bestScore());
}

void MainWindow::showOverlay(const QString &line1, const QString &line2)
{
    hideOverlay();
    m_overlay = new QWidget(m_view);
    m_overlay->setGeometry(m_view->rect());
    m_overlay->setStyleSheet("background: rgba(238, 228, 218, 200);");

    QVBoxLayout *v = new QVBoxLayout(m_overlay);
    v->setAlignment(Qt::AlignCenter);
    QLabel *l1 = new QLabel(line1);
    l1->setAlignment(Qt::AlignCenter);
    l1->setStyleSheet("color:#776e65; font:bold 48px 'Arial'; background:transparent;");
    QLabel *l2 = new QLabel(line2);
    l2->setAlignment(Qt::AlignCenter);
    l2->setStyleSheet("color:#776e65; font:14px 'Arial'; background:transparent;");
    v->addWidget(l1);
    v->addWidget(l2);
    m_overlay->show();
}

void MainWindow::hideOverlay()
{
    if (m_overlay) {
        m_overlay->deleteLater();
        m_overlay = nullptr;
    }
}