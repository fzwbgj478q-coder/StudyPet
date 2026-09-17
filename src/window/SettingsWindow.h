#pragma once
#include <QDialog>
#include "core/AppSettings.h"
class QDoubleSpinBox; class QSpinBox; class QCheckBox; class QLineEdit;
class SettingsWindow final : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    void setSettings(const AppSettings &settings);
signals:
    void settingsApplied(const AppSettings &settings);
private:
    AppSettings collect() const;
    QDoubleSpinBox *m_scale = nullptr, *m_move = nullptr, *m_animation = nullptr;
    QSpinBox *m_volume = nullptr, *m_focus = nullptr, *m_break = nullptr, *m_waterInterval = nullptr, *m_restInterval = nullptr, *m_aiTimeout = nullptr, *m_aiContext = nullptr;
    QCheckBox *m_top = nullptr, *m_click = nullptr, *m_auto = nullptr, *m_bubble = nullptr, *m_startup = nullptr, *m_water = nullptr, *m_rest = nullptr;
    QLineEdit *m_aiBaseUrl = nullptr, *m_aiModel = nullptr;
};

