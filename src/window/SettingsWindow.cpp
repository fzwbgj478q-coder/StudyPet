#include "SettingsWindow.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

SettingsWindow::SettingsWindow(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(QStringLiteral("StudyPet Settings"));
    auto *layout = new QVBoxLayout(this);
    auto *form = new QFormLayout;
    auto doubleSpin = [this](double minimum, double maximum, double value) { auto *box = new QDoubleSpinBox(this); box->setRange(minimum, maximum); box->setSingleStep(0.1); box->setValue(value); return box; };
    auto spin = [this](int minimum, int maximum, int value) { auto *box = new QSpinBox(this); box->setRange(minimum, maximum); box->setValue(value); return box; };
    m_scale=doubleSpin(.5,2,1);m_move=doubleSpin(.25,4,1);m_animation=doubleSpin(.25,4,1);m_volume=spin(0,100,70);m_focus=spin(1,180,25);m_break=spin(1,60,5);m_waterInterval=spin(5,360,45);m_restInterval=spin(10,360,60);
    m_top=new QCheckBox(this);m_click=new QCheckBox(this);m_auto=new QCheckBox(this);m_bubble=new QCheckBox(this);m_startup=new QCheckBox(this);m_water=new QCheckBox(this);m_rest=new QCheckBox(this);
    m_top->setChecked(true);m_auto->setChecked(true);m_bubble->setChecked(true);m_water->setChecked(true);m_rest->setChecked(true);
    form->addRow(QStringLiteral("Pet scale"),m_scale);form->addRow(QStringLiteral("Movement speed"),m_move);form->addRow(QStringLiteral("Animation speed"),m_animation);form->addRow(QStringLiteral("Volume"),m_volume);form->addRow(QStringLiteral("Always on top"),m_top);form->addRow(QStringLiteral("Click through"),m_click);form->addRow(QStringLiteral("Auto activity"),m_auto);form->addRow(QStringLiteral("Proactive bubbles"),m_bubble);form->addRow(QStringLiteral("Start with Windows"),m_startup);form->addRow(QStringLiteral("Focus minutes"),m_focus);form->addRow(QStringLiteral("Break minutes"),m_break);form->addRow(QStringLiteral("Water reminder"),m_water);form->addRow(QStringLiteral("Water interval"),m_waterInterval);form->addRow(QStringLiteral("Rest reminder"),m_rest);form->addRow(QStringLiteral("Rest interval"),m_restInterval);
    layout->addLayout(form);
    auto *aiGroup = new QGroupBox(QStringLiteral("Optional AI (key is never stored here)"), this);
    auto *aiForm = new QFormLayout(aiGroup);
    m_aiBaseUrl=new QLineEdit(aiGroup);m_aiModel=new QLineEdit(aiGroup);m_aiTimeout=spin(5,180,30);m_aiContext=spin(2,100,20);m_aiKeyStatus=new QLabel(aiGroup);
    aiForm->addRow(QStringLiteral("Base URL"),m_aiBaseUrl);aiForm->addRow(QStringLiteral("Model"),m_aiModel);aiForm->addRow(QStringLiteral("Timeout (seconds)"),m_aiTimeout);aiForm->addRow(QStringLiteral("Context messages"),m_aiContext);aiForm->addRow(QStringLiteral("API key"),m_aiKeyStatus);
    layout->addWidget(aiGroup);
    auto *buttons=new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Cancel,this);layout->addWidget(buttons);
    connect(buttons,&QDialogButtonBox::accepted,this,[this]{const auto settings=collect();emit settingsApplied(settings);accept();});connect(buttons,&QDialogButtonBox::rejected,this,&QDialog::reject);
}

void SettingsWindow::setAiKeyConfigured(bool configured)
{
    m_aiKeyStatus->setText(configured ? QStringLiteral("Configured (hidden)") : QStringLiteral("Not configured: offline demo will be used"));
}

void SettingsWindow::setSettings(const AppSettings &settings)
{
    m_scale->setValue(settings.petScale);m_move->setValue(settings.movementSpeed);m_animation->setValue(settings.animationSpeed);m_volume->setValue(settings.volume);m_top->setChecked(settings.alwaysOnTop);m_click->setChecked(settings.clickThrough);m_auto->setChecked(settings.autoActivity);m_bubble->setChecked(settings.proactiveBubbles);m_startup->setChecked(settings.autoStart);m_focus->setValue(settings.focusMinutes);m_break->setValue(settings.breakMinutes);m_water->setChecked(settings.waterReminder);m_waterInterval->setValue(settings.waterIntervalMinutes);m_rest->setChecked(settings.restReminder);m_restInterval->setValue(settings.restIntervalMinutes);m_aiBaseUrl->setText(settings.aiBaseUrl);m_aiModel->setText(settings.aiModel);m_aiTimeout->setValue(settings.aiTimeoutSeconds);m_aiContext->setValue(settings.aiContextLimit);
}

AppSettings SettingsWindow::collect() const
{
    AppSettings settings;settings.petScale=m_scale->value();settings.movementSpeed=m_move->value();settings.animationSpeed=m_animation->value();settings.volume=m_volume->value();settings.alwaysOnTop=m_top->isChecked();settings.clickThrough=m_click->isChecked();settings.autoActivity=m_auto->isChecked();settings.proactiveBubbles=m_bubble->isChecked();settings.autoStart=m_startup->isChecked();settings.focusMinutes=m_focus->value();settings.breakMinutes=m_break->value();settings.waterReminder=m_water->isChecked();settings.waterIntervalMinutes=m_waterInterval->value();settings.restReminder=m_rest->isChecked();settings.restIntervalMinutes=m_restInterval->value();settings.aiBaseUrl=m_aiBaseUrl->text().trimmed();settings.aiModel=m_aiModel->text().trimmed();settings.aiTimeoutSeconds=m_aiTimeout->value();settings.aiContextLimit=m_aiContext->value();settings.sanitize();return settings;
}

