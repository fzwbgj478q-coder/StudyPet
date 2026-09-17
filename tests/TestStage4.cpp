#include <QtTest>

#include <QSignalSpy>
#include <QTemporaryDir>

#include "core/PetStateMachine.h"
#include "service/AiService.h"
#include "service/MockAiClient.h"
#include "service/StorageService.h"

class TestStage4 final : public QObject
{
    Q_OBJECT
private slots:
    void missingKeyUsesMockAndReplies();
    void requestCanBeCancelled();
    void clientFailureIsReported();
    void historyHasConfiguredLimitAndCanClear();
    void chatHistoryRoundTripsWithoutSecrets();
    void talkingStateReturnsToIdle();
};

void TestStage4::missingKeyUsesMockAndReplies()
{
    AiService service;
    service.setApiKeyOverrideForTest(QString());
    QSignalSpy chunks(&service, &AiService::replyChunk);
    QSignalSpy finished(&service, &AiService::replyFinished);
    service.sendUserMessage(QStringLiteral("hello"));
    QVERIFY(service.isUsingMock());
    QTRY_COMPARE(finished.count(), 1);
    QCOMPARE(chunks.count(), 1);
    QCOMPARE(service.history().size(), 2);
    QCOMPARE(static_cast<int>(service.history().last().role), static_cast<int>(ChatRole::Assistant));
}

void TestStage4::requestCanBeCancelled()
{
    AiService service;
    service.setApiKeyOverrideForTest(QString());
    auto *mock = new MockAiClient;
    mock->setDelayForTest(500);
    service.setClientForTest(mock);
    QSignalSpy cancelled(&service, &AiService::replyCancelled);
    service.sendUserMessage(QStringLiteral("cancel"));
    service.cancel();
    QTRY_COMPARE(cancelled.count(), 1);
    QCOMPARE(service.history().size(), 1);
}

void TestStage4::clientFailureIsReported()
{
    AiService service;
    service.setApiKeyOverrideForTest(QString());
    auto *mock = new MockAiClient;
    mock->setFailureForTest(QStringLiteral("network unavailable"));
    service.setClientForTest(mock);
    QSignalSpy failed(&service, &AiService::replyFailed);
    service.sendUserMessage(QStringLiteral("fail"));
    QTRY_COMPARE(failed.count(), 1);
    QCOMPARE(service.history().size(), 1);
}

void TestStage4::historyHasConfiguredLimitAndCanClear()
{
    AiService service;
    AiRequestConfig config; config.baseUrl = QStringLiteral("https://example.invalid/v1"); config.model = QStringLiteral("test"); config.contextLimit = 3;
    service.setConfiguration(config);
    service.setHistory({{ChatRole::User,QStringLiteral("1")},{ChatRole::Assistant,QStringLiteral("2")},{ChatRole::User,QStringLiteral("3")},{ChatRole::Assistant,QStringLiteral("4")}});
    QCOMPARE(service.history().size(), 3);
    QCOMPARE(service.history().first().content, QStringLiteral("2"));
    service.clearHistory();
    QCOMPARE(service.history().size(), 0);
}

void TestStage4::chatHistoryRoundTripsWithoutSecrets()
{
    QTemporaryDir directory;
    StorageService storage(directory.filePath(QStringLiteral("data.json")));
    UserData data;
    data.settings.aiBaseUrl = QStringLiteral("https://example.test/v1");
    data.settings.aiModel = QStringLiteral("demo-model");
    for (int index = 0; index < 25; ++index) data.chatHistory.append({ChatRole::User, QString::number(index)});
    QVERIFY(storage.save(data));
    const UserData loaded = storage.load();
    QCOMPARE(loaded.settings.aiModel, QStringLiteral("demo-model"));
    QCOMPARE(loaded.chatHistory.size(), 20);
}

void TestStage4::talkingStateReturnsToIdle()
{
    PetStateMachine machine;
    QVERIFY(machine.dispatch(PetEvent::AiReplyStarted));
    QCOMPARE(machine.state(), PetState::Talking);
    QVERIFY(machine.dispatch(PetEvent::AiReplyFinished));
    QCOMPARE(machine.state(), PetState::Idle);
}

QTEST_MAIN(TestStage4)
#include "TestStage4.moc"

