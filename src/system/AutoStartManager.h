#pragma once
#include <QString>
class AutoStartManager final { public: static QString valueName(); static QString quotedExecutable(const QString &path); static bool isEnabled(); static bool setEnabled(bool enabled, const QString &executablePath, QString *errorMessage = nullptr); };

