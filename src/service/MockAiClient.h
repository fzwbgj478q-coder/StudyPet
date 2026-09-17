#pragma once

#include "service/IAiClient.h"

class MockAiClient final : public IAiClient
{
    Q_OBJECT
public:
    using IAiClient::IAiClient;
    void startRequest(const QVector<ChatMessage> &messages, const AiRequestConfig &config,
                      const QString &apiKey) override;
    void cancel() override;
    void setFailureForTest(const QString &message) { m_failure = message; }
    void setDelayForTest(int milliseconds) { m_delayMs = milliseconds; }

private:
    bool m_active = false;
    QString m_failure;
    int m_delayMs = 20;
};

