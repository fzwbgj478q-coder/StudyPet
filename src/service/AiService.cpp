#include "AiService.h"

#include "service/IAiClient.h"
#include "service/MockAiClient.h"
#include "service/OpenAiCompatibleClient.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

AiService::AiService(QObject *parent) : QObject(parent)
{
    m_config.baseUrl = QStringLiteral("https://api.openai.com/v1");
    m_config.model = QStringLiteral("gpt-4o-mini");
}

void AiService::setConfiguration(const AiRequestConfig &config)
{
    m_config = config;
    if (m_config.timeoutSeconds < 5 || m_config.timeoutSeconds > 180) m_config.timeoutSeconds = 30;
    if (m_config.contextLimit < 2 || m_config.contextLimit > 100) m_config.contextLimit = 20;
    if (m_config.baseUrl.trimmed().isEmpty()) m_config.baseUrl = QStringLiteral("https://api.openai.com/v1");
    if (m_config.model.trimmed().isEmpty()) m_config.model = QStringLiteral("gpt-4o-mini");
    setHistory(m_history);
}

void AiService::setHistory(const QVector<ChatMessage> &history)
{
    m_history.clear();
    for (const ChatMessage &message : history) {
        if (!message.content.trimmed().isEmpty()) m_history.append(message);
    }
    while (m_history.size() > m_config.contextLimit) m_history.removeFirst();
    emit historyChanged(m_history);
}

bool AiService::isOnlineConfigured() const
{
    return !apiKey().trimmed().isEmpty();
}

void AiService::setClientForTest(IAiClient *client)
{
    if (m_client) m_client->deleteLater();
    m_client = client;
    if (m_client && !m_client->parent()) m_client->setParent(this);
    m_testClient = m_client != nullptr;
    if (m_client) connectClient();
}

void AiService::sendUserMessage(const QString &text)
{
    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty() || m_requestInProgress) return;
    appendHistory(ChatRole::User, trimmed);
    m_assistantBuffer.clear();
    const bool offlineDemo = !isOnlineConfigured();
    if (!m_testClient) createClient(offlineDemo);
    m_usingMock = offlineDemo;
    emit modeChanged(offlineDemo);
    m_requestInProgress = true;
    m_client->startRequest(m_history, m_config, apiKey());
}

void AiService::cancel()
{
    if (m_requestInProgress && m_client) m_client->cancel();
}

void AiService::clearHistory()
{
    if (m_requestInProgress) cancel();
    m_history.clear();
    emit historyChanged(m_history);
}

QString AiService::apiKey() const
{
    if (m_hasApiKeyOverride) return m_apiKeyOverride;
    const QString environmentKey = qEnvironmentVariable("STUDYPET_API_KEY").trimmed();
    if (!environmentKey.isEmpty()) return environmentKey;
    const QStringList locations{QDir::current().filePath(QStringLiteral("config/private.json")),
                                QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("config/private.json"))};
    for (const QString &path : locations) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) continue;
        const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
        const QString key = document.object().value(QStringLiteral("apiKey")).toString().trimmed();
        if (!key.isEmpty()) return key;
    }
    return {};
}

void AiService::createClient(bool offlineDemo)
{
    if (m_client) {
        m_client->deleteLater();
        m_client = nullptr;
    }
    m_usingMock = offlineDemo;
    m_client = offlineDemo ? static_cast<IAiClient *>(new MockAiClient(this))
                           : static_cast<IAiClient *>(new OpenAiCompatibleClient(this));
    connectClient();
}

void AiService::connectClient()
{
    connect(m_client, &IAiClient::responseStarted, this, [this] { emit replyStarted(); });
    connect(m_client, &IAiClient::responseChunk, this, [this](const QString &chunk) {
        m_assistantBuffer += chunk;
        emit replyChunk(chunk);
    });
    connect(m_client, &IAiClient::responseFinished, this, [this] {
        m_requestInProgress = false;
        finishAssistantReply();
        emit replyFinished();
    });
    connect(m_client, &IAiClient::responseFailed, this, [this](const QString &message) {
        m_requestInProgress = false;
        emit replyFailed(message);
    });
    connect(m_client, &IAiClient::responseCancelled, this, [this] {
        m_requestInProgress = false;
        emit replyCancelled();
    });
}

void AiService::appendHistory(ChatRole role, const QString &text)
{
    m_history.append({role, text});
    while (m_history.size() > m_config.contextLimit) m_history.removeFirst();
    emit historyChanged(m_history);
}

void AiService::finishAssistantReply()
{
    if (!m_assistantBuffer.trimmed().isEmpty()) appendHistory(ChatRole::Assistant, m_assistantBuffer);
    m_assistantBuffer.clear();
}

