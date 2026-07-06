#include "SoundManager.h"
#include <QFile>
#include <QStandardPaths>
#include <QUrl>

SoundManager::SoundManager(QObject *parent)
    : QObject(parent)
{
}

SoundManager *SoundManager::instance()
{
    static SoundManager ins;
    return &ins;
}

// 把枚举映射到 qrc 资源路径(用来 dump 到 cache 目录)
static QString qrcPathOf(SoundType type)
{
    switch (type)
    {
    case SoundType::BGM:           return "qrc:/R/music/battle.opus";

    case SoundType::Shoot:         return "qrc:/R/sound/shoot.ogg";
    case SoundType::BulletExplode: return "qrc:/R/sound/bulletExplode.ogg";
    case SoundType::Plant:         return "qrc:/R/sound/plant.ogg";
    case SoundType::PlantDie:      return "qrc:/R/sound/plantDie.ogg";

    case SoundType::ZombieAttack:  return "qrc:/R/sound/zombieAttack.ogg";
    case SoundType::ZombieComing:  return "qrc:/R/sound/zombieComing.ogg";
    case SoundType::HugeWave:      return "qrc:/R/sound/hugeWaveApproching.ogg";
    case SoundType::EvilLaugh:     return "qrc:/R/sound/evillaugh.ogg";

    case SoundType::Win:           return "qrc:/R/sound/win.ogg";
    }
    return {};
}

void SoundManager::init()
{
    if (m_inited) return;
    m_inited = true;

    // 建立 cache 目录,把 qrc 资源 dump 到磁盘,QSoundEffect 走本地路径加载
    // (Windows + FFmpeg 7.x 对 qrc:/... URL 解码有兼容问题,这是 workaround)
    QString cacheRoot = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    m_cacheDir = QDir(cacheRoot + "/PvZLikeSounds");
    if (!m_cacheDir.exists())
    {
        m_cacheDir.mkpath(".");
    }

    // 预创建音效通道
    m_sfxPool.reserve(SFX_CHANNEL_COUNT);
    for (int i = 0; i < SFX_CHANNEL_COUNT; ++i)
    {
        QSoundEffect* fx = new QSoundEffect(this);
        fx->setVolume(1.0f);
        m_sfxPool.append(fx);
    }

    // 背景音乐独占一个通道
    m_bgm = new QSoundEffect(this);
    m_bgm->setLoopCount(QSoundEffect::Infinite);
    m_bgm->setVolume(0.6f);
}

QString SoundManager::ensureLocalFile(SoundType type)
{
    const QString qrcPath = qrcPathOf(type);
    if (qrcPath.isEmpty()) return {};

    // 从 qrc 路径里取出文件名,作为 cache 中的目标文件名
    QString filename = qrcPath;
    int slash = filename.lastIndexOf('/');
    if (slash >= 0) filename = filename.mid(slash + 1);

    const QString localPath = m_cacheDir.filePath(filename);

    // 已存在则直接返回
    if (QFile::exists(localPath)) return localPath;

    // 从 qrc 资源读取原始字节,写到 cache 目录
    QFile src(qrcPath);
    if (!src.open(QIODevice::ReadOnly))
    {
        qWarning("SoundManager: 无法打开 qrc 资源 %s", qPrintable(qrcPath));
        return {};
    }
    const QByteArray data = src.readAll();
    src.close();

    QFile dst(localPath);
    if (!dst.open(QIODevice::WriteOnly))
    {
        qWarning("SoundManager: 无法写入 cache 文件 %s", qPrintable(localPath));
        return {};
    }
    dst.write(data);
    dst.close();

    return localPath;
}

void SoundManager::playSfx(SoundType type, float volume)
{
    if (m_muted || !m_inited) return;
    if (m_sfxPool.isEmpty()) return;

    const QString localPath = ensureLocalFile(type);
    if (localPath.isEmpty()) return;

    // 轮询选一个空闲通道;若都忙就抢当前 cursor(老音会被新音截断,
    // 这对短音反而是对的体验——密集发射时只听到最新的几次)
    QSoundEffect* fx = m_sfxPool[m_sfxCursor];
    m_sfxCursor = (m_sfxCursor + 1) % m_sfxPool.size();

    fx->setSource(QUrl::fromLocalFile(localPath));
    fx->setVolume(qBound(0.0f, volume, 1.0f));
    fx->play();
}

void SoundManager::playBgm(SoundType type, float volume)
{
    if (!m_inited) init();
    if (!m_bgm) return;

    if (m_muted)
    {
        m_bgm->stop();
        return;
    }

    const QString localPath = ensureLocalFile(type);
    if (localPath.isEmpty()) return;

    // 已经在播同一首就不打断
    if (m_bgm->source().toLocalFile() == localPath && m_bgm->isPlaying())
        return;

    m_bgm->setSource(QUrl::fromLocalFile(localPath));
    m_bgm->setVolume(qBound(0.0f, volume, 1.0f));
    m_bgm->play();
}

void SoundManager::stopBgm()
{
    if (m_bgm) m_bgm->stop();
}

void SoundManager::setMuted(bool muted)
{
    m_muted = muted;
    if (muted && m_bgm) m_bgm->stop();
}