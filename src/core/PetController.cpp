#include "PetController.h"

#include "animation/AnimationPlayer.h"
#include "window/PetWindow.h"

#include <QCoreApplication>
#include <QRandomGenerator>
#include <QTimer>

PetController::PetController(QObject *parent)
    : QObject(parent)
    , m_petWindow(new PetWindow)
    , m_animationPlayer(new AnimationPlayer(this))
    , m_idleTimer(new QTimer(this))
    , m_movementTimer(new QTimer(this))
{
    connect(m_petWindow, &PetWindow::exitRequested, this, [] {
        QCoreApplication::quit();
    });
    connect(m_petWindow, &PetWindow::clicked, this, [this] {
        transitionTo(PetState::Clicked);
    });
    connect(m_petWindow, &PetWindow::dragStarted, this, [this] {
        transitionTo(PetState::Dragged);
    });
    connect(m_petWindow, &PetWindow::dragReleased, this, [this] {
        transitionTo(PetState::Idle);
    });
    connect(m_petWindow, &PetWindow::chatRequested, this, [this] {
        m_petWindow->showSpeechBubble(QStringLiteral("你好！聊天功能正在准备中。"));
    });
    connect(m_animationPlayer, &AnimationPlayer::frameChanged,
            m_petWindow, &PetWindow::setPetFrame);
    connect(m_animationPlayer, &AnimationPlayer::actionFinished, this, [this](const QString &) {
        if (m_stateMachine.state() == PetState::Clicked) {
            transitionTo(PetState::Idle);
        }
    });

    m_idleTimer->setSingleShot(true);
    connect(m_idleTimer, &QTimer::timeout, this, &PetController::chooseNextIdleAction);
    m_movementTimer->setInterval(30);
    connect(m_movementTimer, &QTimer::timeout, this, &PetController::movePet);
}

PetController::~PetController()
{
    delete m_petWindow;
}

void PetController::start()
{
    if (!m_animationPlayer->loadConfig(QStringLiteral(":/config/animations.json"))) {
        m_petWindow->showSpeechBubble(QStringLiteral("动画配置不可用，已使用安全占位状态。"), 4000);
    }
    applyCurrentState();
    m_petWindow->show();
}

void PetController::transitionTo(PetState next)
{
    if (m_stateMachine.transitionTo(next)) {
        applyCurrentState();
    }
}

void PetController::applyCurrentState()
{
    const PetState current = m_stateMachine.state();
    m_idleTimer->stop();
    m_movementTimer->stop();

    const QString action = animationForState(current);
    if (m_animationPlayer->setAction(action)) {
        m_animationPlayer->start();
    }

    if (current == PetState::Idle) {
        m_idleTimer->start(QRandomGenerator::global()->bounded(3000, 7001));
    } else if (current == PetState::WalkLeft || current == PetState::WalkRight) {
        m_movementTimer->start();
    }
}

void PetController::chooseNextIdleAction()
{
    if (m_stateMachine.state() != PetState::Idle) {
        return;
    }
    transitionTo(QRandomGenerator::global()->bounded(2) == 0
                     ? PetState::WalkLeft
                     : PetState::WalkRight);
}

void PetController::movePet()
{
    const PetState current = m_stateMachine.state();
    if (current != PetState::WalkLeft && current != PetState::WalkRight) {
        m_movementTimer->stop();
        return;
    }

    const auto result = m_petWindow->moveHorizontally(current == PetState::WalkLeft ? -2 : 2);
    if (result == PetWindow::HorizontalBoundary::Left) {
        transitionTo(PetState::WalkRight);
    } else if (result == PetWindow::HorizontalBoundary::Right) {
        transitionTo(PetState::WalkLeft);
    }
}

QString PetController::animationForState(PetState state)
{
    return QString::fromLatin1(PetStateMachine::name(state));
}

