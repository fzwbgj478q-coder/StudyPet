#pragma once
#include <QObject>
class QTimer;
enum class ReminderKind { Water, Rest };
class ReminderService final : public QObject
{
    Q_OBJECT
public:
    explicit ReminderService(QObject *parent = nullptr);
    void configure(bool water, int waterMinutes, bool rest, int restMinutes);
    void setSuspended(bool suspended);
    void start();
signals:
    void reminderDue(ReminderKind kind);
private:
    QTimer *m_water = nullptr;
    QTimer *m_rest = nullptr;
    bool m_suspended = false;
};

