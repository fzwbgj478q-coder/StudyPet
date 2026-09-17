#pragma once

#include "service/IAiClient.h"

class QNetworkAccessManager;
class QNetworkReply;
class QTimer;

class OpenAiCompatibleClient final : public IAiClient
{
    Q_OBJECT
public:
    explicit OpenAiCompatibleClient(QObject *parent = nullptr);
    void startRequest(const QVector<ChatMessage> &messages, const AiRequestConfig &config,
                      const QString &apiKey) override;
    void cancel() override;

private:
    void consumeStreamData();
    void finishReply();
    void fail(const QString &message);

    QNetworkAccessManager *m_network = nullptr;
    QNetworkReply *m_reply = nullptr;
    QTimer *m_timeout = nullptr;
    QByteArray m_pendingData;
    QString m_responseText;
    bool m_cancelled = false;
    bool m_timedOut = false;
};

