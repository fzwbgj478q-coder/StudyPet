#include "PetStats.h"

#include <QtGlobal>

void PetStats::clamp() { intimacy = qBound(0, intimacy, 100); mood = qBound(0, mood, 100); energy = qBound(0, energy, 100); }
bool PetStats::rewardClick(const QDateTime &now) { if (lastClickReward.isValid() && lastClickReward.secsTo(now) < 30) return false; lastClickReward = now; ++intimacy; clamp(); return true; }
void PetStats::rewardPomodoro() { intimacy += 3; mood += 2; clamp(); }
void PetStats::consumeActivity(int amount) { energy -= amount; clamp(); }
void PetStats::recoverFromSleep(int amount) { energy += amount; clamp(); }
void PetStats::updateDay(const QDate &today) { if (!today.isValid()) return; if (!lastUsedDate.isValid()) { lastUsedDate = today; companionDays = 1; return; } if (lastUsedDate.daysTo(today) == 1) ++companionDays; else if (lastUsedDate != today) companionDays = 1; lastUsedDate = today; }

