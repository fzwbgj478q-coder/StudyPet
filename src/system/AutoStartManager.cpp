#include "AutoStartManager.h"
#include <QDir>
#include <QSettings>
QString AutoStartManager::valueName(){return QStringLiteral("StudyPet");}
QString AutoStartManager::quotedExecutable(const QString &path){return QStringLiteral("\"") + QDir::toNativeSeparators(path) + QStringLiteral("\"");}
bool AutoStartManager::isEnabled(){
#ifdef Q_OS_WIN
 QSettings run(QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"),QSettings::NativeFormat);return run.contains(valueName());
#else
 return false;
#endif
}
bool AutoStartManager::setEnabled(bool enabled,const QString &path,QString *error){
#ifdef Q_OS_WIN
 QSettings run(QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"),QSettings::NativeFormat);if(enabled)run.setValue(valueName(),quotedExecutable(path));else run.remove(valueName());run.sync();if(run.status()!=QSettings::NoError){if(error)*error=QStringLiteral("无法写入当前用户的 Run 注册表项");return false;}return true;
#else
 Q_UNUSED(enabled) Q_UNUSED(path) if(error)*error=QStringLiteral("开机启动仅支持 Windows");return false;
#endif
}

