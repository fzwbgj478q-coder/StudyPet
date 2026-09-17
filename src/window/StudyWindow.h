#pragma once
#include <QDialog>
#include "service/StorageService.h"
class QLabel; class QListWidget; class QLineEdit; class QDateEdit; class QCheckBox;
class StudyWindow final : public QDialog
{
    Q_OBJECT
public:
    explicit StudyWindow(QWidget *parent = nullptr);
    void setData(const UserData &data, const QDate &today);
    void setTimerText(const QString &text);
signals:
    void startRequested();
    void pauseRequested();
    void resumeRequested();
    void resetRequested();
    void todoSaved(const TodoItem &item);
    void todoToggled(const QString &id, bool completed);
    void todoRemoved(const QString &id);
private:
    void loadSelectedTodo();
    QLabel *m_timer = nullptr, *m_today = nullptr, *m_week = nullptr, *m_stats = nullptr;
    QListWidget *m_todos = nullptr;
    QLineEdit *m_title = nullptr;
    QDateEdit *m_due = nullptr;
    QCheckBox *m_hasDue = nullptr;
};

