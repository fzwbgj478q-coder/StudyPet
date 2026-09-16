#include "StudyService.h"
#include <QTimer>
#include <QtGlobal>
StudyService::StudyService(QObject *parent) : QObject(parent), m_timer(new QTimer(this)) { m_timer->setInterval(1000); connect(m_timer, &QTimer::timeout, this, &StudyService::tick); }
void StudyService::setDurations(int focus, int rest) { m_focusMinutes = qBound(1, focus, 180); m_breakMinutes = qBound(1, rest, 60); if (m_state == StudyTimerState::Stopped) resetPhase(); }
void StudyService::setAutoStartBreak(bool enabled) { m_autoStartBreak = enabled; }
void StudyService::start() { if (m_state == StudyTimerState::Stopped) resetPhase(); m_state = StudyTimerState::Running; m_timer->start(); emit stateChanged(m_state); }
void StudyService::pause() { if (m_state == StudyTimerState::Running) { m_timer->stop(); m_state = StudyTimerState::Paused; emit stateChanged(m_state); } }
void StudyService::resume() { if (m_state == StudyTimerState::Paused) { m_state = StudyTimerState::Running; m_timer->start(); emit stateChanged(m_state); } }
void StudyService::reset() { m_timer->stop(); m_state = StudyTimerState::Stopped; m_phase = StudyPhase::Focus; resetPhase(); emit stateChanged(m_state); emit phaseChanged(m_phase); }
void StudyService::tickForTest(int seconds) { while (seconds-- > 0 && m_state == StudyTimerState::Running) tick(); }
int StudyService::remainingSeconds() const { return m_remainingSeconds; } StudyPhase StudyService::phase() const { return m_phase; } StudyTimerState StudyService::state() const { return m_state; }
void StudyService::tick() { if (m_state != StudyTimerState::Running) return; if (--m_remainingSeconds > 0) { emit remainingChanged(m_remainingSeconds); return; } if (m_phase == StudyPhase::Focus) { emit focusCompleted(m_focusMinutes); m_phase = StudyPhase::Break; resetPhase(); emit phaseChanged(m_phase); if (!m_autoStartBreak) { m_timer->stop(); m_state = StudyTimerState::Stopped; emit stateChanged(m_state); } } else { m_phase = StudyPhase::Focus; resetPhase(); emit phaseChanged(m_phase); } }
void StudyService::resetPhase() { m_remainingSeconds = (m_phase == StudyPhase::Focus ? m_focusMinutes : m_breakMinutes) * 60; emit remainingChanged(m_remainingSeconds); }

