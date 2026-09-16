#include "PetWindow.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QContextMenuEvent>
#include <QGuiApplication>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QStyle>
#include <QSystemTrayIcon>
#include <QTimer>

namespace {
constexpr QSize kPetSize{240, 240};
}

PetWindow::PetWindow(QWidget *parent)
    : QWidget(parent)
    , m_currentPixmap(QStringLiteral(":/animations/idle/idle-01.png"))
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
    if (!m_currentPixmap.isNull()) {
        painter.drawPixmap(rect(), m_currentPixmap);
    }
}

void PetWindow::setPetFrame(const QPixmap &frame)
{
    if (!frame.isNull()) {
        m_currentPixmap = frame;
        update();
    }
}

PetWindow::HorizontalBoundary PetWindow::moveHorizontally(int pixels)
{
    const QRect area = currentWorkArea();
    const int requestedX = pos().x() + pixels;
    const int minX = area.left();
    const int maxX = area.right() - width() + 1;
    const int x = qBound(minX, requestedX, maxX);
    move(x, pos().y());

    if (requestedX < minX) {
        return HorizontalBoundary::Left;
    }
    if (requestedX > maxX) {
        return HorizontalBoundary::Right;
    }
    return HorizontalBoundary::None;
}

void PetWindow::showSpeechBubble(const QString &text, int durationMs)
{
    auto *bubble = new QLabel(text, this);
    bubble->setWordWrap(true);
    bubble->setAlignment(Qt::AlignCenter);
    bubble->setGeometry(24, 8, width() - 48, 52);
    bubble->setStyleSheet(QStringLiteral(
        "QLabel { background: rgba(255, 255, 255, 230); color: #3b2b52; "
        "border: 1px solid #8d73b2; border-radius: 12px; padding: 4px; }"));
    bubble->show();
    QTimer::singleShot(durationMs, bubble, &QObject::deleteLater);
}

void PetWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_pointerPressed = true;
        m_dragging = false;
        m_dragStartGlobal = event->globalPosition().toPoint();
        m_windowStart = pos();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void PetWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_pointerPressed && (event->buttons() & Qt::LeftButton)) {
        if (!m_dragging
            && (event->globalPosition().toPoint() - m_dragStartGlobal).manhattanLength()
                >= QApplication::startDragDistance()) {
            m_dragging = true;
            emit dragStarted();
        }
        move(m_windowStart + event->globalPosition().toPoint() - m_dragStartGlobal);
        keepInsideCurrentScreen();
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void PetWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_pointerPressed) {
        m_pointerPressed = false;
        keepInsideCurrentScreen();
        if (m_dragging) {
            m_dragging = false;
            emit dragReleased();
        } else {
            emit clicked();
        }
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void PetWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit chatRequested();
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
    const QRect area = currentWorkArea();
    const int x = qBound(area.left(), pos().x(), area.right() - width() + 1);
    const int y = qBound(area.top(), pos().y(), area.bottom() - height() + 1);
    move(x, y);
}

QRect PetWindow::currentWorkArea() const
{
    QScreen *screen = QGuiApplication::screenAt(frameGeometry().center());
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }
    return screen->availableGeometry();
}

void PetWindow::setClickThrough(bool enabled)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, enabled);
    setWindowFlag(Qt::WindowTransparentForInput, enabled);
    show();
}

