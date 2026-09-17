#include "OpenAiCompatibleClient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>

namespace {
QString endpointFor(const QString &baseUrl)
{
    QUrl url(baseUrl);
    QString path = url.path();
    if (!path.endsWith(QStringLiteral("/chat/completions"))) {
        if (!path.endsWith(QLatin1Char('/'))) path += QLatin1Char('/');
        path += QStringLiteral("chat/completions");
    }
    url.setPath(path);
    return url.toString();
}

QString errorFromJson(const QByteArray &body)
{
    const QJsonDocument document = QJsonDocument::fromJson(body);
    const QJsonObject error = document.object().value(QStringLiteral("error")).toObject();
    return error.value(QStringLiteral("message")).toString();
}
}

OpenAiCompatibleClient::OpenAiCompatibleClient(QObject *parent)
    : IAiClient(parent)
    , m_network(new QNetworkAccessManager(this))
    , m_timeout(new QTimer(this))
{
    m_timeout->setSingleShot(true);
    connect(m_timeout, &QTimer::timeout, this, [this] {
        if (m_reply) {
            m_timedOut = true;
            m_reply->abort();
        }
    });
}

void OpenAiCompatibleClient::startRequest(const QVector<ChatMessage> &messages,
                                          const AiRequestConfig &config, const QString &apiKey)
{
    if (m_reply) return;
    QJsonArray jsonMessages;
    for (const ChatMessage &message : messages) {
        jsonMessages.append(QJsonObject{{QStringLiteral("role"), chatRoleName(message.role)},
                                        {QStringLiteral("content"), message.content}});
    }
    const QJsonObject payload{{QStringLiteral("model"), config.model},
                              {QStringLiteral("messages"), jsonMessages},
                              {QStringLiteral("stream"), true}};
    QNetworkRequest request{QUrl(endpointFor(config.baseUrl))};
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("Authorization", QByteArray("Bearer ") + apiKey.toUtf8());
    m_cancelled = false;
    m_timedOut = false;
    m_pendingData.clear();
    m_responseText.clear();
    m_reply = m_network->post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    connect(m_reply, &QNetworkReply::readyRead, this, &OpenAiCompatibleClient::consumeStreamData);
    connect(m_reply, &QNetworkReply::finished, this, &OpenAiCompatibleClient::finishReply);
    m_timeout->start(config.timeoutSeconds * 1000);
    emit responseStarted();
}

void OpenAiCompatibleClient::cancel()
{
    if (!m_reply) return;
    m_cancelled = true;
    m_timeout->stop();
    m_reply->abort();
}

void OpenAiCompatibleClient::consumeStreamData()
{
    if (!m_reply) return;
    m_pendingData += m_reply->readAll();
    while (true) {
        const int lineEnd = m_pendingData.indexOf('\n');
        if (lineEnd < 0) break;
        const QByteArray line = m_pendingData.left(lineEnd).trimmed();
        m_pendingData.remove(0, lineEnd + 1);
        if (!line.startsWith("data:")) continue;
        const QByteArray data = line.mid(5).trimmed();
        if (data == "[DONE]") continue;
        const QJsonDocument document = QJsonDocument::fromJson(data);
        const QJsonArray choices = document.object().value(QStringLiteral("choices")).toArray();
        if (choices.isEmpty()) continue;
        const QJsonObject choice = choices.at(0).toObject();
        const QString chunk = choice.value(QStringLiteral("delta")).toObject().value(QStringLiteral("content")).toString();
        if (!chunk.isEmpty()) {
            m_responseText += chunk;
            emit responseChunk(chunk);
        }
    }
}

void OpenAiCompatibleClient::finishReply()
{
    if (!m_reply) return;
    m_timeout->stop();
    QNetworkReply *reply = m_reply;
    m_reply = nullptr;
    const QByteArray body = reply->readAll();
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const auto networkError = reply->error();
    reply->deleteLater();
    if (m_cancelled) {
        emit responseCancelled();
        return;
    }
    if (m_timedOut) {
        fail(QStringLiteral("AI 请求超时，请检查网络或稍后重试。"));
        return;
    }
    if (networkError != QNetworkReply::NoError) {
        fail(QStringLiteral("AI 连接失败：%1").arg(reply->errorString()));
        return;
    }
    if (status < 200 || status >= 300) {
        const QString detail = errorFromJson(body);
        fail(detail.isEmpty() ? QStringLiteral("AI 服务返回 HTTP %1。") .arg(status)
                              : QStringLiteral("AI 服务错误：%1").arg(detail));
        return;
    }
    if (m_responseText.isEmpty()) {
        const QJsonDocument document = QJsonDocument::fromJson(body);
        const QJsonArray choices = document.object().value(QStringLiteral("choices")).toArray();
        const QString full = choices.isEmpty() ? QString() : choices.at(0).toObject()
                                 .value(QStringLiteral("message")).toObject().value(QStringLiteral("content")).toString();
        if (full.isEmpty()) {
            fail(QStringLiteral("AI 返回的数据格式无法识别。"));
            return;
        }
        m_responseText = full;
        emit responseChunk(full);
    }
    emit responseFinished();
}

void OpenAiCompatibleClient::fail(const QString &message)
{
    emit responseFailed(message);
}

