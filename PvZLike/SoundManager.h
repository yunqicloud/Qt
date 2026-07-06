#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QSoundEffect>
#include <QString>
#include <QList>
#include <QDir>

// 可识别的音效/音乐类型(只保留项目中实际会触发的)
// 资源路径通过 sfxPath() / bgmPath() 映射到 Resource.qrc
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

// 全局短别名,和 Mgr 一个用法
#define SoundMgr SoundManager::instance()

class SoundManager : public QObject
{
    Q_OBJECT
public:
    static SoundManager* instance();

    // 启动时调用一次:把 qrc 资源 dump 到 cache 目录,创建音效通道
    void init();

    // 播放一次性音效。channel 个独立通道,密集触发时轮询,
    // 避免前一个还没播完被截断
    void playSfx(SoundType type, float volume = 1.0f);

    // 播放/循环背景音乐
    void playBgm(SoundType type, float volume = 0.6f);

    // 停止背景音乐
    void stopBgm();

    // 静默所有声音
    void setMuted(bool muted);
    bool isMuted() const { return m_muted; }

private:
    explicit SoundManager(QObject *parent = nullptr);

    // 把 qrc 里的音频文件 dump 到 cache 目录(第一次播放某文件时)
    QString ensureLocalFile(SoundType type);

    // 短音效池
    QList<QSoundEffect*> m_sfxPool;
    QSoundEffect* m_bgm = nullptr;

    // 用于本地路径加载(QSoundEffect 在 Windows + FFmpeg 7.x 上
    // 直接 load qrc:/... 会报 Error decoding;先把 qrc 数据 dump 到
    // cache 目录再走本地路径,稳定可靠)
    QDir m_cacheDir;

    int m_sfxCursor = 0;
    bool m_muted = false;
    bool m_inited = false;

    static constexpr int SFX_CHANNEL_COUNT = 8;
};

#endif // SOUNDMANAGER_H