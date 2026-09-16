#pragma once

#include <QObject>

class QTimer;
enum class StudyPhase { Focus, Break };
enum class StudyTimerState { Stopped, Running, Paused };
class StudyService final : public QObject {
    Q_OBJECT
public:
    explicit StudyService(QObject *parent = nullptr);
    void setDurations(int focusMinutes, int breakMinutes);
    void setAutoStartBreak(bool enabled);
    void start(); void pause(); void resume(); void reset(); void tickForTest(int seconds = 1);
    [[nodiscard]] int remainingSeconds() const; [[nodiscard]] StudyPhase phase() const; [[nodiscard]] StudyTimerState state() const;
signals:
    void remainingChanged(int seconds); void stateChanged(StudyTimerState state); void phaseChanged(StudyPhase phase); void focusCompleted(int minutes);
private:
    void tick(); void resetPhase();
    QTimer *m_timer; int m_focusMinutes = 25; int m_breakMinutes = 5; int m_remainingSeconds = 25 * 60; bool m_autoStartBreak = true; StudyPhase m_phase = StudyPhase::Focus; StudyTimerState m_state = StudyTimerState::Stopped;
};

