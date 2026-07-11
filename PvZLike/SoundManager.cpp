#include "SoundManager.h"
#include <QCoreApplication>
#include <QFile>

// 项目内资源根目录。
// CMake 在编译时把 PVL_R_DIR 展开成源码 R/ 的绝对路径,所有音频加载走这里,
// 不依赖运行时拷贝。如果宏未定义(罕见),回退到 exe 同级 R/(分发场景)。
static QString rRoot()
{
#ifdef PVL_R_DIR
    return QString(PVL_R_DIR);
#else
    return QCoreApplication::applicationDirPath() + "/R";
#endif
}

namespace {
// 每个 SoundType 分配的并发通道数。BGM 1 个,高频音效给 4 个应对密集触发。
QMap<SoundType, int> defaultAllocations()
{
    return {
        { SoundType::BGM,           1 },   // 背景音乐:1 个,循环,够用
        { SoundType::Shoot,         4 },   // 豌豆发射:并发可能 4+
        { SoundType::BulletExplode, 4 },   // 豌豆爆炸:并发可能多
        { SoundType::ZombieAttack,  4 },   // 啃咬声:多僵尸同时啃咬
        { SoundType::ZombieComing,  1 },   // 进场提示:只播一次
        { SoundType::Plant,         1 },
        { SoundType::PlantDie,      1 },
        { SoundType::HugeWave,      1 },
        { SoundType::EvilLaugh,     1 },
        { SoundType::Win,           1 },
    };
}
} // namespace

SoundManager::SoundManager(QObject *parent) : QObject(parent) {}

SoundManager::~SoundManager()
{
    for (auto it = m_channels.constBegin(); it != m_channels.constEnd(); ++it)
        for (const Channel& ch : it.value())
            if (ch.player) ch.player->stop();
}

SoundManager *SoundManager::instance()
{
    static SoundManager ins;
    return &ins;
}

QString SoundManager::resolvePath(SoundType type) const
{
    const QString base = rRoot();
    switch (type)
    {
    case SoundType::BGM:           return base + "/music/battle.opus";
    case SoundType::Shoot:         return base + "/sound/shoot.ogg";
    case SoundType::BulletExplode: return base + "/sound/bulletExplode.ogg";
    case SoundType::Plant:         return base + "/sound/plant.ogg";
    case SoundType::PlantDie:      return base + "/sound/plantDie.ogg";
    case SoundType::ZombieAttack:  return base + "/sound/zombieAttack.ogg";
    case SoundType::ZombieComing:  return base + "/sound/zombieComing.ogg";
    case SoundType::HugeWave:      return base + "/sound/hugeWaveApproching.ogg";
    case SoundType::EvilLaugh:     return base + "/sound/evillaugh.ogg";
    case SoundType::Win:           return base + "/sound/win.ogg";
    }
    return {};
}

void SoundManager::initTypeChannels(SoundType type, int count, bool loop)
{
    const QString localPath = resolvePath(type);
    if (localPath.isEmpty()) return;

    // 文件不存在就跳过(不报警,某些 SoundType 可能还没启用)
    if (!QFile::exists(localPath))
    {
        qWarning("SoundManager: 音频文件不存在,跳过 %s", qPrintable(localPath));
        return;
    }

    QList<Channel> list;
    list.reserve(count);
    for (int i = 0; i < count; ++i)
    {
        Channel ch;
        ch.player = new QMediaPlayer(this);
        ch.output = new QAudioOutput(this);
        ch.player->setAudioOutput(ch.output);
        ch.player->setLoops(loop ? QMediaPlayer::Infinite : 1);
        // 一次性 setSource,启动时建立好 FFmpeg 流,运行时不再 setSource
        ch.player->setSource(QUrl::fromLocalFile(localPath));
        list.append(ch);
    }
    m_channels.insert(type, list);
    m_cursors.insert(type, 0);
}

void SoundManager::init()
{
    if (m_inited) return;
    m_inited = true;

    const auto allocs = defaultAllocations();
    for (auto it = allocs.constBegin(); it != allocs.constEnd(); ++it)
    {
        const SoundType type = it.key();
        const int count = it.value();
        const bool loop = (type == SoundType::BGM);  // BGM 循环,其他单次
        initTypeChannels(type, count, loop);
    }
}

void SoundManager::playSfx(SoundType type, float volume)
{
    if (m_muted || !m_inited) return;

    auto it = m_channels.find(type);
    if (it == m_channels.end() || it.value().isEmpty()) return;

    QList<Channel>& list = it.value();
    int& cursor = m_cursors[type];

    // 优先找空闲 channel(playbackState == StoppedState 表示已播完)
    int chosen = -1;
    int baseIdx = cursor;
    for (int i = 0; i < list.size(); ++i)
    {
        const int idx = (baseIdx + i) % list.size();
        if (list[idx].player->playbackState() != QMediaPlayer::PlayingState)
        {
            chosen = idx;
            break;
        }
    }
    if (chosen < 0)
    {
        // 都忙,轮询抢一个
        chosen = cursor;
    }

    cursor = (chosen + 1) % list.size();
    Channel& ch = list[chosen];

    ch.output->setVolume(qBound(0.0f, volume, 1.0f));
    // 关键:stop() 让 QMediaPlayer 重置到 StoppedState + position=0,
    // 再 play() 自动从 0 开始;source 不需要重新 LoadingMedia (init 期已 loaded),
    // 所以不会触发 "Input #0, ogg, from" 重新探测。
    // setPosition(0) 在 EndOfMedia 状态下行为不可靠,改用 stop() 更稳。
    ch.player->stop();
    ch.player->play();
}

void SoundManager::playBgm(SoundType type, float volume)
{
    if (!m_inited) init();

    if (m_muted) { stopBgm(); return; }

    auto it = m_channels.find(type);
    if (it == m_channels.end() || it.value().isEmpty()) return;

    Channel& ch = it.value().first();
    if (!ch.player || !ch.output) return;

    ch.output->setVolume(qBound(0.0f, volume, 1.0f));
    ch.player->stop();
    ch.player->play();
}

void SoundManager::stopBgm()
{
    auto it = m_channels.find(SoundType::BGM);
    if (it == m_channels.end() || it.value().isEmpty()) return;
    if (it.value().first().player) it.value().first().player->stop();
}

void SoundManager::setMuted(bool muted)
{
    m_muted = muted;
    if (muted) stopBgm();
}