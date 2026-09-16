#include <QtTest>
#include <QFile>
#include <QTemporaryDir>
#include "core/AppSettings.h"
#include "core/PetStats.h"
#include "service/StudyService.h"
#include "service/StorageService.h"
class TestStage3 final: public QObject { Q_OBJECT private slots: void settingsClamp(); void statsRewardsAreBounded(); void clickThrottle(); void pomodoroCanBeDrivenWithoutWaiting(); void storageRoundTripAndTodo(); void corruptJsonFallsBack(); };
void TestStage3::settingsClamp(){AppSettings s;s.petScale=99;s.focusMinutes=0;s.volume=999;s.sanitize();QCOMPARE(s.petScale,1.0);QCOMPARE(s.focusMinutes,25);QCOMPARE(s.volume,70);}
void TestStage3::statsRewardsAreBounded(){PetStats s;s.intimacy=99;s.mood=99;s.energy=1;s.rewardPomodoro();s.consumeActivity(99);QCOMPARE(s.intimacy,100);QCOMPARE(s.mood,100);QCOMPARE(s.energy,0);}
void TestStage3::clickThrottle(){PetStats s;auto now=QDateTime::currentDateTimeUtc();QVERIFY(s.rewardClick(now));QVERIFY(!s.rewardClick(now.addSecs(29)));QVERIFY(s.rewardClick(now.addSecs(30)));}
void TestStage3::pomodoroCanBeDrivenWithoutWaiting(){StudyService s;s.setDurations(1,1);s.start();QSignalSpy done(&s,&StudyService::focusCompleted);s.tickForTest(60);QCOMPARE(done.count(),1);QCOMPARE(static_cast<int>(s.phase()),static_cast<int>(StudyPhase::Break));}
void TestStage3::storageRoundTripAndTodo(){QTemporaryDir dir;StorageService store(dir.filePath("data.json"));UserData data;data.settings.animationSpeed=2.0;data.settings.restIntervalMinutes=90;data.stats.intimacy=42;StorageService::upsertTodo(data,{QString(),"写测试"});StorageService::addFocus(data,QDate(2026,9,16),25);QVERIFY(store.save(data));auto loaded=store.load();QCOMPARE(loaded.settings.animationSpeed,2.0);QCOMPARE(loaded.settings.restIntervalMinutes,90);QCOMPARE(loaded.stats.intimacy,42);QCOMPARE(loaded.todos.size(),1);QCOMPARE(StorageService::focusMinutes(loaded,QDate(2026,9,16)),25);}
void TestStage3::corruptJsonFallsBack(){QTemporaryDir dir;auto path=dir.filePath("data.json");QFile f(path);QVERIFY(f.open(QIODevice::WriteOnly));f.write("not json");f.close();StorageService store(path);auto data=store.load();QCOMPARE(data.settings.focusMinutes,25);QCOMPARE(data.todos.size(),0);}
QTEST_MAIN(TestStage3)
#include "TestStage3.moc"

