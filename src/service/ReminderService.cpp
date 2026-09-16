#include "ReminderService.h"
#include <QTimer>
ReminderService::ReminderService(QObject *p):QObject(p),m_water(new QTimer(this)),m_rest(new QTimer(this)){ connect(m_water,&QTimer::timeout,this,[this]{if(!m_suspended) emit reminderDue(ReminderKind::Water);}); connect(m_rest,&QTimer::timeout,this,[this]{if(!m_suspended) emit reminderDue(ReminderKind::Rest);}); }
void ReminderService::configure(bool water,int wm,bool rest,int rm){ m_water->stop();m_rest->stop();if(water)m_water->start(qBound(5,wm,360)*60000);if(rest)m_rest->start(qBound(10,rm,360)*60000); } void ReminderService::setSuspended(bool s){m_suspended=s;} void ReminderService::start(){if(m_water->interval())m_water->start();if(m_rest->interval())m_rest->start();}

