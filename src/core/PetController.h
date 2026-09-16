#pragma once

#include <QObject>
#include <QString>

#include "PetStateMachine.h"

class AnimationPlayer;
class PetWindow;
class QTimer;

/// Coordinates application-level lifecycle without putting it in the window class.
class PetController final : public QObject
{
    Q_OBJECT

public:
    explicit PetController(QObject *parent = nullptr);
    ~PetController() override;
    void start();

private:
    void transitionTo(PetState next);
    void applyCurrentState();
    void chooseNextIdleAction();
    void movePet();
    [[nodiscard]] static QString animationForState(PetState state);

    PetWindow *m_petWindow = nullptr;
    AnimationPlayer *m_animationPlayer = nullptr;
    QTimer *m_idleTimer = nullptr;
    QTimer *m_movementTimer = nullptr;
    PetStateMachine m_stateMachine;
};

