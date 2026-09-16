#include "PetWindow.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QContextMenuEvent>
#include <QGuiApplication>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QStyle>
#include <QSystemTrayIcon>

namespace {
constexpr QSize kPetSize{240, 240};
}

PetWindow::PetWindow(QWidget *parent)
    : QWidget(parent)
    , m_idlePixmap(QStringLiteral(":/animations/idle/idle-01.png"))
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setFixedSize(kPetSize);
    setWindowTitle(QStringLiteral("StudyPet"));

    const QRect workArea = QGuiApplication::primaryScreen()->availableGeometry();
    move(workArea.right() - width() - 24, workArea.bottom() - height() - 24);
    createTrayMenu();
}

void PetWindow::createTrayMenu()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    m_trayIcon->setToolTip(QStringLiteral("StudyPet — 学习陪伴桌宠"));

    m_trayMenu = new QMenu(this);
    auto *showAction = m_trayMenu->addAction(QStringLiteral("显示桌宠"));
    auto *hideAction = m_trayMenu->addAction(QStringLiteral("隐藏桌宠"));
    m_trayMenu->addSeparator();
    m_clickThroughAction = m_trayMenu->addAction(QStringLiteral("鼠标点击穿透"));
    m_clickThroughAction->setCheckable(true);
    m_trayMenu->addSeparator();
    auto *exitAction = m_trayMenu->addAction(QStringLiteral("退出 StudyPet"));

    connect(showAction, &QAction::triggered, this, [this] { show(); raise(); activateWindow(); });
    connect(hideAction, &QAction::triggered, this, &QWidget::hide);
    connect(m_clickThroughAction, &QAction::toggled, this, &PetWindow::setClickThrough);
    connect(exitAction, &QAction::triggered, this, [this] {
        m_isQuitting = true;
        emit exitRequested();
    });
    connect(m_trayIcon, &QSystemTrayIcon::activated, this,
            [this](QSystemTrayIcon::ActivationReason reason) {
                if (reason == QSystemTrayIcon::Trigger) {
                    isVisible() ? hide() : show();
                }
            });

    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->show();
}

void PetWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    if (!m_idlePixmap.isNull()) {
        painter.drawPixmap(rect(), m_idlePixmap);
    }
}

void PetWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartGlobal = event->globalPosition().toPoint();
        m_windowStart = pos();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void PetWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(m_windowStart + event->globalPosition().toPoint() - m_dragStartGlobal);
        keepInsideCurrentScreen();
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void PetWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_dragging) {
        m_dragging = false;
        keepInsideCurrentScreen();
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void PetWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_trayIcon->showMessage(QStringLiteral("StudyPet"),
                                QStringLiteral("聊天功能将在第 4 阶段开放。"),
                                QSystemTrayIcon::Information, 2500);
        event->accept();
        return;
    }
    QWidget::mouseDoubleClickEvent(event);
}

void PetWindow::contextMenuEvent(QContextMenuEvent *event)
{
    m_trayMenu->exec(event->globalPos());
}

void PetWindow::closeEvent(QCloseEvent *event)
{
    if (m_isQuitting) {
        event->accept();
        return;
    }
    hide();
    event->ignore();
}

void PetWindow::keepInsideCurrentScreen()
{
    QScreen *screen = QGuiApplication::screenAt(frameGeometry().center());
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }
    const QRect area = screen->availableGeometry();
    const int x = qBound(area.left(), pos().x(), area.right() - width() + 1);
    const int y = qBound(area.top(), pos().y(), area.bottom() - height() + 1);
    move(x, y);
}

void PetWindow::setClickThrough(bool enabled)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, enabled);
    setWindowFlag(Qt::WindowTransparentForInput, enabled);
    show();
}

