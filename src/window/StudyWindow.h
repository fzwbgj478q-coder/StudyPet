#pragma once
#include <QDialog>
#include "service/StorageService.h"
class QLabel; class QListWidget; class QLineEdit; class QDateEdit; class QCheckBox;
class StudyWindow final: public QDialog { Q_OBJECT public: explicit StudyWindow(QWidget*parent=nullptr); void setData(const UserData&, const QDate&today); void setTimerText(const QString&); signals: void startRequested();void pauseRequested();void resumeRequested();void resetRequested();void todoSaved(const TodoItem&);void todoToggled(const QString&,bool);void todoRemoved(const QString&); private: void loadSelectedTodo(); QLabel *m_timer,*m_today,*m_week,*m_stats; QListWidget*m_todos;QLineEdit*m_title;QDateEdit*m_due;QCheckBox*m_hasDue; };

