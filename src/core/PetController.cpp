#include "PetController.h"

#include "animation/AnimationPlayer.h"
#include "service/ReminderService.h"
#include "service/StorageService.h"
#include "service/StudyService.h"
#include "window/PetWindow.h"
#include "window/SettingsWindow.h"

#include <QCoreApplication>
#include <QDate>
#include <QRandomGenerator>
#include <QTimer>

PetController::PetController(QObject *parent)
    : QObject(parent)
    , m_petWindow(new PetWindow)
    , m_animationPlayer(new AnimationPlayer(this))
    , m_idleTimer(new QTimer(this))
    , m_movementTimer(new QTimer(this))
    , m_storage(new StorageService)
    , m_userData(new UserData(m_storage->load()))
    , m_studyService(new StudyService(this))
    , m_reminderService(new ReminderService(this))
    , m_settingsWindow(new SettingsWindow)
{
    m_userData->stats.updateDay(QDate::currentDate());
    connect(m_petWindow, &PetWindow::exitRequested, this, [] {
        QCoreApplication::quit();
    });
    connect(m_petWindow, &PetWindow::clicked, this, [this] {
        if (m_userData->stats.rewardClick(QDateTime::currentDateTime())) {
            m_storage->save(*m_userData);
        }
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
    connect(m_petWindow, &PetWindow::settingsRequested, this, [this] { m_settingsWindow->setSettings(m_userData->settings); m_settingsWindow->show(); m_settingsWindow->raise(); });
    connect(m_petWindow, &PetWindow::studyRequested, this, [this] { m_studyService->start(); m_petWindow->showSpeechBubble(QStringLiteral("已开始番茄钟专注。")); });
    connect(m_petWindow, &PetWindow::statsRequested, this, [this] { const auto &s=m_userData->stats; m_petWindow->showSpeechBubble(QStringLiteral("亲密度 %1｜心情 %2｜体力 %3").arg(s.intimacy).arg(s.mood).arg(s.energy), 4000); });
    connect(m_settingsWindow, &SettingsWindow::settingsApplied, this, [this](const AppSettings &settings) { m_userData->settings=settings; m_studyService->setDurations(settings.focusMinutes,settings.breakMinutes); m_reminderService->configure(settings.waterReminder,settings.waterIntervalMinutes,settings.restReminder,settings.restIntervalMinutes); m_storage->save(*m_userData); });
    connect(m_studyService, &StudyService::focusCompleted, this, [this](int minutes) { StorageService::addFocus(*m_userData,QDate::currentDate(),minutes);m_userData->stats.rewardPomodoro();m_storage->save(*m_userData);if(m_animationPlayer->setAction(QStringLiteral("Happy")))m_animationPlayer->start();m_petWindow->showSpeechBubble(QStringLiteral("专注完成，做得好！")); });
    connect(m_reminderService,&ReminderService::reminderDue,this,[this](ReminderKind kind){m_petWindow->showSpeechBubble(kind==ReminderKind::Water?QStringLiteral("该喝水啦！"):QStringLiteral("起来活动一下吧！"));});
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
    m_storage->save(*m_userData);
    delete m_settingsWindow;
    delete m_userData;
    delete m_storage;
    delete m_petWindow;
}

void PetController::start()
{
    m_studyService->setDurations(m_userData->settings.focusMinutes,m_userData->settings.breakMinutes);
    m_reminderService->configure(m_userData->settings.waterReminder,m_userData->settings.waterIntervalMinutes,m_userData->settings.restReminder,m_userData->settings.restIntervalMinutes);
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

