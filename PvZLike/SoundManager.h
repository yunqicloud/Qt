#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QString>
#include <QList>
#include <QMap>

// 可识别的音效/音乐类型
enum class SoundType
{
    BGM,                  // 背景音乐(循环)

    Shoot,                // 豌豆发射
    BulletExplode,        // 豌豆爆炸(命中僵尸)

    ZombieAttack,         // 僵尸啃咬植物
    ZombieComing,         // 僵尸进场提示

    Plant,                // 种植物
    PlantDie,             // 植物被吃掉

    HugeWave,             // 大波次来袭
    EvilLaugh,            // 胜利前笑声
    Win,                  // 胜利
};

#define SoundMgr SoundManager::instance()

class SoundManager : public QObject
{
    Q_OBJECT
public:
    static SoundManager* instance();

    // 启动时调用:为每个 SoundType 预创建 N 个 channel,一次性 setSource,
    // 之后所有播放都只调 setPosition(0) + play()(完全不碰 setSource,
    // 避免运行时反复重建 FFmpeg 解码器引发警告/卡顿/崩溃)
    void init();

    void playSfx(SoundType type, float volume = 1.0f);
    void playBgm(SoundType type, float volume = 0.6f);

    void stopBgm();
    void setMuted(bool muted);
    bool isMuted() const { return m_muted; }

private:
    explicit SoundManager(QObject *parent = nullptr);
    ~SoundManager() override;

    QString resolvePath(SoundType type) const;

    // 单个音频通道
    struct Channel
    {
        QMediaPlayer*  player = nullptr;
        QAudioOutput*  output = nullptr;
    };

    void initTypeChannels(SoundType type, int count, bool loop);

    // 每个 SoundType 一组 channel。运行时只从对应组里选一个空闲的 setPosition+play
    QMap<SoundType, QList<Channel>> m_channels;
    QMap<SoundType, int>          m_cursors;     // 每个 SoundType 自己的轮询游标

    bool m_muted = false;
    bool m_inited = false;
};

#endif // SOUNDMANAGER_H