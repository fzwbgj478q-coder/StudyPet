#pragma once

#include <QObject>
#include <QHash>
#include <QPixmap>
#include <QStringList>

class QTimer;

struct AnimationDefinition {
    QStringList frames;
    int intervalMs = 250;
    bool loop = true;
};

/// Loads declarative PNG animations and advances frames through the Qt event loop.
class AnimationPlayer final : public QObject
{
    Q_OBJECT

public:
    explicit AnimationPlayer(QObject *parent = nullptr);

    [[nodiscard]] bool loadConfig(const QString &configPath);
    [[nodiscard]] bool setAction(const QString &actionName);
    [[nodiscard]] bool hasAction(const QString &actionName) const;
    [[nodiscard]] QString currentAction() const;
    [[nodiscard]] QStringList actionNames() const;
    [[nodiscard]] bool isRunning() const;

public slots:
    void start();
    void pause();
    void stop();

signals:
    void frameChanged(const QPixmap &frame);
    void actionChanged(const QString &actionName);
    void actionFinished(const QString &actionName);

private slots:
    void advanceFrame();

private:
    [[nodiscard]] bool emitCurrentFrame();
    [[nodiscard]] static bool frameExists(const QString &path);

    QHash<QString, AnimationDefinition> m_animations;
    QTimer *m_timer = nullptr;
    QString m_currentAction;
    int m_currentFrame = 0;
    bool m_running = false;
};

