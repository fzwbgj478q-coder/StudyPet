#include "PetController.h"

#include "animation/AnimationPlayer.h"
#include "service/AiService.h"
#include "service/ReminderService.h"
#include "service/StorageService.h"
#include "service/StudyService.h"
#include "system/AutoStartManager.h"
#include "window/PetWindow.h"
#include "window/ChatWindow.h"
#include "window/SettingsWindow.h"
#include "window/StudyWindow.h"

#include <QCoreApplication>
#include <QDate>
#include <QRandomGenerator>
#include <QTimer>
#include <QtGlobal>

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
    , m_aiService(new AiService(this))
    , m_settingsWindow(new SettingsWindow)
    , m_studyWindow(new StudyWindow)
    , m_chatWindow(new ChatWindow)
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
    connect(m_petWindow, &PetWindow::settingsRequested, this, [this] { m_userData->settings.autoStart=AutoStartManager::isEnabled();m_settingsWindow->setSettings(m_userData->settings);m_settingsWindow->setAiKeyConfigured(m_aiService->isOnlineConfigured());m_settingsWindow->show();m_settingsWindow->raise(); });
    connect(m_petWindow, &PetWindow::studyRequested, this, [this] { m_studyWindow->setData(*m_userData,QDate::currentDate());m_studyWindow->show();m_studyWindow->raise(); });
    connect(m_petWindow, &PetWindow::statsRequested, this, [this] { const auto &s=m_userData->stats; m_petWindow->showSpeechBubble(QStringLiteral("亲密度 %1｜心情 %2｜体力 %3").arg(s.intimacy).arg(s.mood).arg(s.energy), 4000); });
    connect(m_settingsWindow, &SettingsWindow::settingsApplied, this, [this](const AppSettings &settings) { m_userData->settings=settings; QString error; if(!AutoStartManager::setEnabled(settings.autoStart,QCoreApplication::applicationFilePath(),&error) && settings.autoStart)m_petWindow->showSpeechBubble(error); m_petWindow->setPetScale(settings.petScale);m_petWindow->setAlwaysOnTop(settings.alwaysOnTop);m_petWindow->setClickThroughEnabled(settings.clickThrough);m_animationPlayer->setSpeedMultiplier(settings.animationSpeed);m_studyService->setDurations(settings.focusMinutes,settings.breakMinutes);m_reminderService->configure(settings.waterReminder,settings.waterIntervalMinutes,settings.restReminder,settings.restIntervalMinutes);m_aiService->setConfiguration({settings.aiBaseUrl,settings.aiModel,settings.aiTimeoutSeconds,settings.aiContextLimit});m_storage->save(*m_userData); });
    connect(m_studyService, &StudyService::focusCompleted, this, [this](int minutes) { StorageService::addFocus(*m_userData,QDate::currentDate(),minutes);m_userData->stats.rewardPomodoro();m_storage->save(*m_userData);if(m_animationPlayer->setAction(QStringLiteral("Happy")))m_animationPlayer->start();m_petWindow->showSpeechBubble(QStringLiteral("专注完成，做得好！")); });
    connect(m_studyService,&StudyService::remainingChanged,this,[this](int seconds){m_studyWindow->setTimerText(QStringLiteral("%1:%2").arg(seconds/60,2,10,QChar('0')).arg(seconds%60,2,10,QChar('0')));});
    connect(m_studyWindow,&StudyWindow::startRequested,m_studyService,&StudyService::start);connect(m_studyWindow,&StudyWindow::pauseRequested,m_studyService,&StudyService::pause);connect(m_studyWindow,&StudyWindow::resumeRequested,m_studyService,&StudyService::resume);connect(m_studyWindow,&StudyWindow::resetRequested,m_studyService,&StudyService::reset);
    connect(m_studyWindow,&StudyWindow::todoSaved,this,[this](TodoItem item){StorageService::upsertTodo(*m_userData,item);m_storage->save(*m_userData);m_studyWindow->setData(*m_userData,QDate::currentDate());});
    connect(m_studyWindow,&StudyWindow::todoToggled,this,[this](const QString&id,bool done){for(auto&t:m_userData->todos)if(t.id==id)t.completed=done;m_storage->save(*m_userData);});connect(m_studyWindow,&StudyWindow::todoRemoved,this,[this](const QString&id){StorageService::removeTodo(*m_userData,id);m_storage->save(*m_userData);m_studyWindow->setData(*m_userData,QDate::currentDate());});
    connect(m_reminderService,&ReminderService::reminderDue,this,[this](ReminderKind kind){if(m_userData->settings.proactiveBubbles)m_petWindow->showSpeechBubble(kind==ReminderKind::Water?QStringLiteral("该喝水啦！"):QStringLiteral("起来活动一下吧！"));});
    connect(m_petWindow, &PetWindow::chatRequested, this, [this] { m_chatWindow->show(); m_chatWindow->raise(); m_chatWindow->activateWindow(); });
    connect(m_petWindow, &PetWindow::aiChatRequested, this, [this] { m_chatWindow->show(); m_chatWindow->raise(); m_chatWindow->activateWindow(); });
    connect(m_chatWindow, &ChatWindow::messageSubmitted, this, [this](const QString &text) { m_chatWindow->appendUserMessage(text); m_chatWindow->setStatus(QStringLiteral("Thinking")); m_aiService->sendUserMessage(text); });
    connect(m_chatWindow, &ChatWindow::cancelRequested, m_aiService, &AiService::cancel);
    connect(m_chatWindow, &ChatWindow::clearRequested, this, [this] { m_aiService->clearHistory(); m_chatWindow->clearTranscript(); });
    connect(m_aiService, &AiService::modeChanged, m_chatWindow, &ChatWindow::setMode);
    connect(m_aiService, &AiService::replyStarted, this, [this] { if (m_stateMachine.state() != PetState::Idle) transitionTo(PetState::Idle); m_stateMachine.dispatch(PetEvent::AiReplyStarted); applyCurrentState(); m_chatWindow->beginAssistantMessage(); m_chatWindow->setStatus(QStringLiteral("Generating")); });
    connect(m_aiService, &AiService::replyChunk, this, [this](const QString &chunk) { m_chatWindow->appendAssistantChunk(chunk); if (m_userData->settings.proactiveBubbles) m_petWindow->showSpeechBubble(chunk.left(48), 1500); });
    connect(m_aiService, &AiService::replyFinished, this, [this] { m_stateMachine.dispatch(PetEvent::AiReplyFinished); applyCurrentState(); m_chatWindow->setStatus(QStringLiteral("Ready")); });
    connect(m_aiService, &AiService::replyCancelled, this, [this] { m_stateMachine.dispatch(PetEvent::AiReplyFinished); applyCurrentState(); m_chatWindow->setStatus(QStringLiteral("Generation stopped")); });
    connect(m_aiService, &AiService::replyFailed, this, [this](const QString &message) { m_stateMachine.dispatch(PetEvent::AiReplyFinished); applyCurrentState(); m_chatWindow->appendError(message); m_chatWindow->setStatus(QStringLiteral("Connection failed")); });
    connect(m_aiService, &AiService::historyChanged, this, [this](const QVector<ChatMessage> &history) { m_userData->chatHistory = history; m_storage->save(*m_userData); });
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
    delete m_studyWindow;
    delete m_chatWindow;
    delete m_userData;
    delete m_storage;
    delete m_petWindow;
}

void PetController::start()
{
    m_studyService->setDurations(m_userData->settings.focusMinutes,m_userData->settings.breakMinutes);
    m_aiService->setConfiguration({m_userData->settings.aiBaseUrl,m_userData->settings.aiModel,m_userData->settings.aiTimeoutSeconds,m_userData->settings.aiContextLimit});
    m_aiService->setHistory(m_userData->chatHistory);
    for (const ChatMessage &message : m_userData->chatHistory) {
        if (message.role == ChatRole::User) {
            m_chatWindow->appendUserMessage(message.content);
        } else if (message.role == ChatRole::Assistant) {
            m_chatWindow->beginAssistantMessage();
            m_chatWindow->appendAssistantChunk(message.content);
        }
    }
    m_chatWindow->setMode(!m_aiService->isOnlineConfigured());
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

    if (current == PetState::Idle && m_userData->settings.autoActivity) {
        m_idleTimer->start(QRandomGenerator::global()->bounded(3000, 7001));
    } else if (current == PetState::WalkLeft || current == PetState::WalkRight) {
        m_movementTimer->start();
    }
}

void PetController::chooseNextIdleAction()
{
    if (m_stateMachine.state() != PetState::Idle || !m_userData->settings.autoActivity) {
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

    const int pixels=qMax(1,qRound(2*m_userData->settings.movementSpeed));
    const auto result = m_petWindow->moveHorizontally(current == PetState::WalkLeft ? -pixels : pixels);
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

