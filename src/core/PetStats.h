#pragma once

#include <QDate>
#include <QDateTime>

struct PetStats {
    int intimacy = 0;
    int mood = 60;
    int energy = 100;
    int companionDays = 1;
    QDate lastUsedDate;
    QDateTime lastClickReward;

    void clamp();
    bool rewardClick(const QDateTime &now);
    void rewardPomodoro();
    void consumeActivity(int amount = 1);
    void recoverFromSleep(int amount = 10);
    void updateDay(const QDate &today);
};

