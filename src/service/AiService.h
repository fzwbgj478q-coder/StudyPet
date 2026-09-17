#pragma once

#include <QObject>

#include "service/AiTypes.h"

class IAiClient;

class AiService final : public QObject
{
    Q_OBJECT
public:
    explicit AiService(QObject *parent = nullptr);
    void setConfiguration(const AiRequestConfig &config);
    void setHistory(const QVector<ChatMessage> &history);
    [[nodiscard]] const QVector<ChatMessage> &history() const { return m_history; }
    [[nodiscard]] bool isOnlineConfigured() const;
    [[nodiscard]] bool isUsingMock() const { return m_usingMock; }
    void setApiKeyOverrideForTest(const QString &key) { m_apiKeyOverride = key; m_hasApiKeyOverride = true; }
    void setClientForTest(IAiClient *client);
    void sendUserMessage(const QString &text);
    void cancel();
    void clearHistory();

signals:
    void modeChanged(bool offlineDemo);
    void replyStarted();
    void replyChunk(const QString &text);
    void replyFinished();
    void replyFailed(const QString &message);
    void replyCancelled();
    void historyChanged(const QVector<ChatMessage> &history);

private:
    [[nodiscard]] QString apiKey() const;
    void createClient(bool offlineDemo);
    void connectClient();
    void appendHistory(ChatRole role, const QString &text);
    void finishAssistantReply();

    AiRequestConfig m_config;
    QVector<ChatMessage> m_history;
    IAiClient *m_client = nullptr;
    QString m_assistantBuffer;
    QString m_apiKeyOverride;
    bool m_hasApiKeyOverride = false;
    bool m_usingMock = true;
    bool m_requestInProgress = false;
    bool m_testClient = false;
};

