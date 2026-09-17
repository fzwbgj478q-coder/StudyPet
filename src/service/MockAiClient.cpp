#include "MockAiClient.h"

#include <QTimer>

void MockAiClient::startRequest(const QVector<ChatMessage> &, const AiRequestConfig &, const QString &)
{
    if (m_active) return;
    m_active = true;
    emit responseStarted();
    QTimer::singleShot(m_delayMs, this, [this] {
        if (!m_active) return;
        m_active = false;
        if (!m_failure.isEmpty()) {
            emit responseFailed(m_failure);
            return;
        }
        emit responseChunk(QStringLiteral("你好，我是 StudyPet。目前处于离线演示模式。"));
        emit responseFinished();
    });
}

void MockAiClient::cancel()
{
    if (!m_active) return;
    m_active = false;
    emit responseCancelled();
}

