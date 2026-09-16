#pragma once

/// Framework-independent behavioural states for the desktop pet.
enum class PetState {
    Idle,
    WalkLeft,
    WalkRight,
    Sleep,
    Dragged,
    Clicked,
    Talking
};

enum class PetEvent {
    StartWalkLeft,
    StartWalkRight,
    GoToSleep,
    UserClicked,
    DragStarted,
    DragReleased,
    ReachedLeftEdge,
    ReachedRightEdge,
    StopMoving,
    AnimationFinished,
    AiReplyStarted,
    AiReplyFinished,
    WakeUp
};

/// Pure C++ transition table. It has no Qt or window dependency and is unit-testable.
class PetStateMachine final
{
public:
    [[nodiscard]] PetState state() const noexcept;
    [[nodiscard]] bool transitionTo(PetState next) noexcept;
    [[nodiscard]] bool dispatch(PetEvent event) noexcept;
    [[nodiscard]] static const char *name(PetState state) noexcept;

private:
    PetState m_state = PetState::Idle;
};

