#pragma once

#include <QPixmap>
#include <QPoint>
#include <QWidget>

class QAction;
class QMenu;
class QSystemTrayIcon;

/// A transparent, draggable top-level desktop-pet window.
class PetWindow final : public QWidget
{
    Q_OBJECT

public:
    explicit PetWindow(QWidget *parent = nullptr);

signals:
    void exitRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void createTrayMenu();
    void keepInsideCurrentScreen();
    void setClickThrough(bool enabled);

    QPixmap m_idlePixmap;
    QPoint m_dragStartGlobal;
    QPoint m_windowStart;
    bool m_dragging = false;
    bool m_isQuitting = false;
    QSystemTrayIcon *m_trayIcon = nullptr;
    QMenu *m_trayMenu = nullptr;
    QAction *m_clickThroughAction = nullptr;
};

