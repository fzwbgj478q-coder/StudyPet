#pragma once

#include <QDialog>
class QLabel; class QPlainTextEdit; class QPushButton;
class QEvent;
class ChatWindow final : public QDialog
{
    Q_OBJECT
public:
    explicit ChatWindow(QWidget *parent = nullptr);
    void setMode(bool offlineDemo); void setStatus(const QString &status); void appendUserMessage(const QString &text); void beginAssistantMessage(); void appendAssistantChunk(const QString &text); void appendError(const QString &text); void clearTranscript();
signals:
    void messageSubmitted(const QString &text); void cancelRequested(); void clearRequested();
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    void appendBubble(const QString &speaker, const QString &text);
    QLabel *m_mode = nullptr, *m_status = nullptr; QPlainTextEdit *m_transcript = nullptr, *m_input = nullptr; QPushButton *m_send = nullptr; QString m_assistantText;
};

