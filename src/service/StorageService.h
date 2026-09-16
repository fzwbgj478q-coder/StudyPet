#pragma once
#include "core/AppSettings.h"
#include "core/PetStats.h"
#include <QDate>
#include <QDateTime>
#include <QString>
#include <QVector>
struct TodoItem { QString id; QString title; bool completed=false; QDateTime createdAt; QDate dueDate; };
struct DailyStudyRecord { QDate date; int focusMinutes=0; int pomodoros=0; };
struct UserData { AppSettings settings; PetStats stats; QVector<TodoItem> todos; QVector<DailyStudyRecord> history; };
class StorageService final { public: [[nodiscard]] QString dataPath() const; [[nodiscard]] UserData load() const; [[nodiscard]] bool save(const UserData &data) const; static void addFocus(UserData &data, const QDate &date, int minutes); };

