#pragma once

#include <QObject>

#include "service/AiTypes.h"

class IAiClient : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
    ~IAiClient() override = default;
    virtual void startRequest(const QVector<ChatMessage> &messages,
                              const AiRequestConfig &config,
                              const QString &apiKey) = 0;
    virtual void cancel() = 0;

signals:
    void responseStarted();
    void responseChunk(const QString &text);
    void responseFinished();
    void responseFailed(const QString &message);
    void responseCancelled();
};

