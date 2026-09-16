#include "PetStateMachine.h"

PetState PetStateMachine::state() const noexcept
{
    return m_state;
}

bool PetStateMachine::transitionTo(PetState next) noexcept
{
    if (next == m_state) {
        return true;
    }

    bool allowed = false;
    switch (m_state) {
    case PetState::Idle:
        allowed = next == PetState::WalkLeft || next == PetState::WalkRight || next == PetState::Sleep
            || next == PetState::Dragged || next == PetState::Clicked || next == PetState::Talking;
        break;
    case PetState::WalkLeft:
        allowed = next == PetState::Idle || next == PetState::WalkRight || next == PetState::Dragged
            || next == PetState::Clicked;
        break;
    case PetState::WalkRight:
        allowed = next == PetState::Idle || next == PetState::WalkLeft || next == PetState::Dragged
            || next == PetState::Clicked;
        break;
    case PetState::Sleep:
        allowed = next == PetState::Idle || next == PetState::Dragged || next == PetState::Clicked;
        break;
    case PetState::Dragged:
        allowed = next == PetState::Idle;
        break;
    case PetState::Clicked:
        allowed = next == PetState::Idle || next == PetState::Dragged;
        break;
    case PetState::Talking:
        allowed = next == PetState::Idle || next == PetState::Dragged;
        break;
    }

    if (allowed) {
        m_state = next;
    }
    return allowed;
}

bool PetStateMachine::dispatch(PetEvent event) noexcept
{
    switch (event) {
    case PetEvent::StartWalkLeft: return transitionTo(PetState::WalkLeft);
    case PetEvent::StartWalkRight: return transitionTo(PetState::WalkRight);
    case PetEvent::GoToSleep: return transitionTo(PetState::Sleep);
    case PetEvent::UserClicked: return transitionTo(PetState::Clicked);
    case PetEvent::DragStarted: return transitionTo(PetState::Dragged);
    case PetEvent::DragReleased:
    case PetEvent::StopMoving:
    case PetEvent::AnimationFinished:
    case PetEvent::AiReplyFinished:
    case PetEvent::WakeUp:
        return transitionTo(PetState::Idle);
    case PetEvent::ReachedLeftEdge:
        return m_state == PetState::WalkLeft && transitionTo(PetState::WalkRight);
    case PetEvent::ReachedRightEdge:
        return m_state == PetState::WalkRight && transitionTo(PetState::WalkLeft);
    case PetEvent::AiReplyStarted: return transitionTo(PetState::Talking);
    }
    return false;
}

const char *PetStateMachine::name(PetState state) noexcept
{
    switch (state) {
    case PetState::Idle: return "Idle";
    case PetState::WalkLeft: return "WalkLeft";
    case PetState::WalkRight: return "WalkRight";
    case PetState::Sleep: return "Sleep";
    case PetState::Dragged: return "Dragged";
    case PetState::Clicked: return "Clicked";
    case PetState::Talking: return "Talking";
    }
    return "Unknown";
}

