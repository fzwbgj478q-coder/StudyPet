#include "AnimationPlayer.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QImageReader>
#include <QTimer>
#include <QTransform>
#include <QtGlobal>

AnimationPlayer::AnimationPlayer(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &AnimationPlayer::advanceFrame);
}

bool AnimationPlayer::loadConfig(const QString &configPath)
{
    QFile configFile(configPath);
    if (!configFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open animation config:" << configPath;
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(configFile.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        qWarning() << "Invalid animation JSON:" << configPath << parseError.errorString();
        return false;
    }

    const QJsonObject actions = document.object().value(QStringLiteral("actions")).toObject();
    if (actions.isEmpty()) {
        qWarning() << "Animation config has no actions:" << configPath;
        return false;
    }

    QHash<QString, AnimationDefinition> parsedAnimations;
    for (auto it = actions.begin(); it != actions.end(); ++it) {
        if (!it.value().isObject()) {
            qWarning() << "Invalid animation entry:" << it.key();
            continue;
        }
        const QJsonObject object = it.value().toObject();
        const int intervalMs = object.value(QStringLiteral("intervalMs")).toInt(-1);
        const QJsonArray jsonFrames = object.value(QStringLiteral("frames")).toArray();
        if (intervalMs < 16 || intervalMs > 60000 || jsonFrames.isEmpty()) {
            qWarning() << "Animation has invalid interval or empty frames:" << it.key();
            continue;
        }

        AnimationDefinition definition;
        definition.intervalMs = intervalMs;
        definition.loop = object.value(QStringLiteral("loop")).toBool(true);
        definition.mirror = object.value(QStringLiteral("mirror")).toBool(false);
        const auto collectFrames = [this, &it](const QJsonArray &frames, QStringList *target) {
            target->clear();
            if (frames.isEmpty()) {
                return false;
            }
            for (const QJsonValue &value : frames) {
                const QString framePath = value.toString();
                if (framePath.isEmpty() || !frameExists(framePath)) {
                    qWarning() << "Animation frame is unavailable:" << framePath << "for" << it.key();
                    target->clear();
                    return false;
                }
                target->append(framePath);
            }
            return true;
        };
        const QJsonArray fallbackJsonFrames = object.value(QStringLiteral("fallbackFrames")).toArray();
        const bool hasPrimaryFrames = collectFrames(jsonFrames, &definition.frames);
        const bool hasFallbackFrames = collectFrames(fallbackJsonFrames, &definition.fallbackFrames);
        if (!hasPrimaryFrames && hasFallbackFrames) {
            qWarning() << "Using fallback animation frames for" << it.key();
            definition.frames = definition.fallbackFrames;
            definition.fallbackFrames.clear();
        }
        if (hasPrimaryFrames || hasFallbackFrames) {
            parsedAnimations.insert(it.key(), definition);
        }
    }

    if (parsedAnimations.isEmpty()) {
        qWarning() << "Animation config contains no usable actions:" << configPath;
        return false;
    }

    stop();
    m_animations = parsedAnimations;
    m_currentAction.clear();
    return true;
}

bool AnimationPlayer::setAction(const QString &actionName)
{
    const auto iterator = m_animations.constFind(actionName);
    if (iterator == m_animations.cend()) {
        qWarning() << "Unknown animation action:" << actionName;
        return false;
    }

    const bool restart = m_running;
    m_timer->stop();
    m_currentAction = actionName;
    m_currentFrame = 0;
    m_running = restart;
    emit actionChanged(m_currentAction);
    if (!emitCurrentFrame()) {
        return false;
    }
    if (m_running) {
        m_timer->start(iterator->intervalMs);
    }
    return true;
}

bool AnimationPlayer::hasAction(const QString &actionName) const
{
    return m_animations.contains(actionName);
}

QString AnimationPlayer::currentAction() const
{
    return m_currentAction;
}

QStringList AnimationPlayer::actionNames() const
{
    return m_animations.keys();
}

bool AnimationPlayer::isRunning() const
{
    return m_running;
}
void AnimationPlayer::setSpeedMultiplier(double multiplier) { m_speedMultiplier = qBound(0.25, multiplier, 4.0); if (m_running && m_animations.contains(m_currentAction)) m_timer->start(qRound(m_animations.value(m_currentAction).intervalMs / m_speedMultiplier)); }

void AnimationPlayer::start()
{
    if (m_currentAction.isEmpty() || !m_animations.contains(m_currentAction)) {
        qWarning() << "Cannot start animation without a selected action.";
        return;
    }
    m_running = true;
    if (!emitCurrentFrame()) {
        stop();
        return;
    }
    m_timer->start(qRound(m_animations.value(m_currentAction).intervalMs / m_speedMultiplier));
}

void AnimationPlayer::pause()
{
    m_timer->stop();
    m_running = false;
}

void AnimationPlayer::stop()
{
    m_timer->stop();
    m_running = false;
    m_currentFrame = 0;
}

void AnimationPlayer::advanceFrame()
{
    const auto iterator = m_animations.constFind(m_currentAction);
    if (iterator == m_animations.cend() || iterator->frames.isEmpty()) {
        qWarning() << "Cannot advance an unavailable animation action:" << m_currentAction;
        stop();
        return;
    }

    ++m_currentFrame;
    if (m_currentFrame >= iterator->frames.size()) {
        if (!iterator->loop) {
            const QString finishedAction = m_currentAction;
            stop();
            emit actionFinished(finishedAction);
            return;
        }
        m_currentFrame = 0;
    }
    emitCurrentFrame();
}

bool AnimationPlayer::emitCurrentFrame()
{
    auto iterator = m_animations.find(m_currentAction);
    if (iterator == m_animations.end() || m_currentFrame < 0
        || m_currentFrame >= iterator->frames.size()) {
        qWarning() << "Current animation frame is invalid for action:" << m_currentAction;
        return false;
    }

    const QPixmap frame(iterator->frames.at(m_currentFrame));
    if (frame.isNull()) {
        qWarning() << "Unable to decode animation frame:" << iterator->frames.at(m_currentFrame);
        if (iterator->fallbackFrames.isEmpty()) {
            return false;
        }
        qWarning() << "Falling back to placeholder animation for" << m_currentAction;
        iterator->frames = iterator->fallbackFrames;
        iterator->fallbackFrames.clear();
        m_currentFrame = 0;
        return emitCurrentFrame();
    }
    emit frameChanged(iterator->mirror
                          ? frame.transformed(QTransform().scale(-1.0, 1.0), Qt::SmoothTransformation)
                          : frame);
    return true;
}

bool AnimationPlayer::frameExists(const QString &path)
{
    QImageReader reader(path);
    return QFile::exists(path) && reader.canRead();
}

