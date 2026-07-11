#include <QCoreApplication>
#include <QApplication>
#include <QDebug>
#include <cstdio>
#include "Game.h"

// 自定义消息 handler:把 FFmpeg 解码器的"discarded samples" /
// "skipped samples" 时间戳警告吞掉 (这些是 vorbis/opus 解码器在
// QMediaPlayer::stop()+play() 重放时的良性输出,无害但刷屏影响阅读体验)
// 其他消息原样输出到 stderr。
static void pvzMessageHandler(QtMsgType type, const QMessageLogContext& /*ctx*/, const QString& msg)
{
    if (msg.contains("[vorbis @") || msg.contains("[opus @") ||
        msg.contains("Could not update timestamps"))
    {
        return;  // 吞掉 FFmpeg 解码器的时间戳警告
    }

    const char* levelName = "DEBUG";
    switch (type)
    {
    case QtDebugMsg:    levelName = "DEBUG"; break;
    case QtInfoMsg:     levelName = "INFO";  break;
    case QtWarningMsg:  levelName = "WARN";  break;
    case QtCriticalMsg: levelName = "CRIT";  break;
    case QtFatalMsg:    levelName = "FATAL"; break;
    }
    fprintf(stderr, "[%s] %s\n", levelName, msg.toLocal8Bit().constData());
    fflush(stderr);
}


int main(int argc, char *argv[])
{
    qInstallMessageHandler(pvzMessageHandler);

    //QCoreApplication a(argc, argv);
    QApplication a(argc, argv);

    Game game;
    game.initGame(1400, 600, "PvZ");
    game.run();

    return a.exec();
}