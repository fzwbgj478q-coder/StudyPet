#include <QtTest>

#include <QFile>
#include <QTemporaryDir>

#include "animation/AnimationPlayer.h"
#include "core/PetStateMachine.h"

class TestStage2 final : public QObject
{
    Q_OBJECT

private slots:
    void legalStateTransitions();
    void illegalStateTransitionsAreRejected();
    void animationConfigLoadsFromResources();
    void missingAnimationResourceIsHandled();
    void missingPrimaryAnimationUsesFallback();
};

void TestStage2::legalStateTransitions()
{
    PetStateMachine stateMachine;
    QCOMPARE(static_cast<int>(stateMachine.state()), static_cast<int>(PetState::Idle));
    QVERIFY(stateMachine.dispatch(PetEvent::StartWalkLeft));
    QCOMPARE(static_cast<int>(stateMachine.state()), static_cast<int>(PetState::WalkLeft));
    QVERIFY(stateMachine.dispatch(PetEvent::ReachedLeftEdge));
    QCOMPARE(static_cast<int>(stateMachine.state()), static_cast<int>(PetState::WalkRight));
    QVERIFY(stateMachine.dispatch(PetEvent::DragStarted));
    QCOMPARE(static_cast<int>(stateMachine.state()), static_cast<int>(PetState::Dragged));
    QVERIFY(stateMachine.dispatch(PetEvent::DragReleased));
    QCOMPARE(static_cast<int>(stateMachine.state()), static_cast<int>(PetState::Idle));
    QVERIFY(stateMachine.dispatch(PetEvent::UserClicked));
    QCOMPARE(static_cast<int>(stateMachine.state()), static_cast<int>(PetState::Clicked));
    QVERIFY(stateMachine.dispatch(PetEvent::AnimationFinished));
    QCOMPARE(static_cast<int>(stateMachine.state()), static_cast<int>(PetState::Idle));
}

void TestStage2::illegalStateTransitionsAreRejected()
{
    PetStateMachine stateMachine;
    QVERIFY(stateMachine.dispatch(PetEvent::StartWalkLeft));
    QVERIFY(!stateMachine.transitionTo(PetState::Sleep));
    QCOMPARE(static_cast<int>(stateMachine.state()), static_cast<int>(PetState::WalkLeft));
    QVERIFY(!stateMachine.dispatch(PetEvent::ReachedRightEdge));
    QCOMPARE(static_cast<int>(stateMachine.state()), static_cast<int>(PetState::WalkLeft));
}

void TestStage2::animationConfigLoadsFromResources()
{
    AnimationPlayer player;
    QVERIFY(player.loadConfig(QStringLiteral(":/config/animations.json")));
    QVERIFY(player.hasAction(QStringLiteral("Idle")));
    QVERIFY(player.hasAction(QStringLiteral("Clicked")));
    QVERIFY(player.setAction(QStringLiteral("Idle")));
    QCOMPARE(player.currentAction(), QStringLiteral("Idle"));
    player.start();
    QVERIFY(player.isRunning());
    player.pause();
    QVERIFY(!player.isRunning());
}

void TestStage2::missingAnimationResourceIsHandled()
{
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());
    const QString configPath = temporaryDirectory.filePath(QStringLiteral("missing-frame.json"));
    QFile file(configPath);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write(R"({"actions":{"Broken":{"frames":[":/not-found.png"],"intervalMs":100,"loop":true}}})");
    file.close();

    AnimationPlayer player;
    QVERIFY(!player.loadConfig(configPath));
    QVERIFY(!player.setAction(QStringLiteral("Broken")));
}

void TestStage2::missingPrimaryAnimationUsesFallback()
{
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());
    const QString configPath = temporaryDirectory.filePath(QStringLiteral("fallback-frame.json"));
    QFile file(configPath);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write(R"({"actions":{"Recover":{"frames":[":/not-found.png"],"fallbackFrames":[":/animations/idle/idle-01.png"],"intervalMs":100,"loop":true}}})");
    file.close();

    AnimationPlayer player;
    QVERIFY(player.loadConfig(configPath));
    QVERIFY(player.setAction(QStringLiteral("Recover")));
}

QTEST_MAIN(TestStage2)
#include "TestStage2.moc"

