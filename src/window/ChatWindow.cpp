#include "ChatWindow.h"
#include <QEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextCursor>
#include <QVBoxLayout>
ChatWindow::ChatWindow(QWidget *parent):QDialog(parent){setWindowTitle(QStringLiteral("StudyPet AI Chat"));resize(460,540);auto*layout=new QVBoxLayout(this);m_mode=new QLabel(this);m_status=new QLabel(QStringLiteral("Ready"),this);layout->addWidget(m_mode);layout->addWidget(m_status);m_transcript=new QPlainTextEdit(this);m_transcript->setReadOnly(true);layout->addWidget(m_transcript,1);m_input=new QPlainTextEdit(this);m_input->setPlaceholderText(QStringLiteral("Type a message. Enter sends; Shift+Enter makes a new line."));m_input->setFixedHeight(74);m_input->installEventFilter(this);layout->addWidget(m_input);auto*buttons=new QHBoxLayout;m_send=new QPushButton(QStringLiteral("Send"),this);auto*stop=new QPushButton(QStringLiteral("Stop"),this);auto*clear=new QPushButton(QStringLiteral("Clear conversation"),this);buttons->addWidget(m_send);buttons->addWidget(stop);buttons->addWidget(clear);layout->addLayout(buttons);connect(m_send,&QPushButton::clicked,this,[this]{const auto text=m_input->toPlainText().trimmed();if(text.isEmpty())return;m_input->clear();emit messageSubmitted(text);});connect(stop,&QPushButton::clicked,this,&ChatWindow::cancelRequested);connect(clear,&QPushButton::clicked,this,&ChatWindow::clearRequested);setMode(true);}
void ChatWindow::setMode(bool offlineDemo){m_mode->setText(offlineDemo?QStringLiteral("Mode: Offline demo"):QStringLiteral("Mode: Online AI"));}
void ChatWindow::setStatus(const QString&status){m_status->setText(QStringLiteral("Status: %1").arg(status));}
void ChatWindow::appendBubble(const QString&speaker,const QString&text){m_transcript->appendPlainText(QStringLiteral("%1: %2\n").arg(speaker,text));m_transcript->moveCursor(QTextCursor::End);}
void ChatWindow::appendUserMessage(const QString&text){appendBubble(QStringLiteral("You"),text);}
void ChatWindow::beginAssistantMessage(){m_assistantText.clear();appendBubble(QStringLiteral("StudyPet"),QString());}
void ChatWindow::appendAssistantChunk(const QString&text){m_assistantText+=text;auto cursor=m_transcript->textCursor();cursor.movePosition(QTextCursor::End);cursor.select(QTextCursor::BlockUnderCursor);cursor.removeSelectedText();cursor.insertText(QStringLiteral("StudyPet: %1").arg(m_assistantText));m_transcript->setTextCursor(cursor);m_transcript->moveCursor(QTextCursor::End);}
void ChatWindow::appendError(const QString&text){appendBubble(QStringLiteral("System"),text);}
void ChatWindow::clearTranscript(){m_assistantText.clear();m_transcript->clear();}
bool ChatWindow::eventFilter(QObject*watched,QEvent*event){if(watched==m_input&&event->type()==QEvent::KeyPress){auto*key=static_cast<QKeyEvent*>(event);if((key->key()==Qt::Key_Return||key->key()==Qt::Key_Enter)&&!(key->modifiers()&Qt::ShiftModifier)){m_send->click();return true;}}return QDialog::eventFilter(watched,event);}

