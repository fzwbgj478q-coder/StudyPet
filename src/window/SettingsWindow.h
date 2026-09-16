#pragma once
#include <QDialog>
#include "core/AppSettings.h"
class QDoubleSpinBox; class QSpinBox; class QCheckBox;
class SettingsWindow final: public QDialog { Q_OBJECT public: explicit SettingsWindow(QWidget *parent=nullptr); void setSettings(const AppSettings&); signals: void settingsApplied(const AppSettings&); private: AppSettings collect() const; QDoubleSpinBox *m_scale,*m_move,*m_animation; QSpinBox *m_volume,*m_focus,*m_break,*m_waterInterval,*m_restInterval; QCheckBox *m_top,*m_click,*m_auto,*m_bubble,*m_startup,*m_water,*m_rest; };

