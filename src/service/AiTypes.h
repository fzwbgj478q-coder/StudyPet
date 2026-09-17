#pragma once

#include <QString>
#include <QVector>

enum class ChatRole { User, Assistant, System };

struct ChatMessage
{
    ChatRole role = ChatRole::User;
    QString content;
};

struct AiRequestConfig
{
    QString baseUrl;
    QString model;
    int timeoutSeconds = 30;
    int contextLimit = 20;
};

inline QString chatRoleName(ChatRole role)
{
    switch (role) {
    case ChatRole::User: return QStringLiteral("user");
    case ChatRole::Assistant: return QStringLiteral("assistant");
    case ChatRole::System: return QStringLiteral("system");
    }
    return QStringLiteral("user");
}

inline ChatRole chatRoleFromName(const QString &name, bool *valid = nullptr)
{
    if (valid) *valid = true;
    if (name == QStringLiteral("user")) return ChatRole::User;
    if (name == QStringLiteral("assistant")) return ChatRole::Assistant;
    if (name == QStringLiteral("system")) return ChatRole::System;
    if (valid) *valid = false;
    return ChatRole::User;
}

