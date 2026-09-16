#pragma once

#include <QPixmap>
#include <QPoint>
#include <QRect>
#include <QWidget>

class QAction;
class QMenu;
class QSystemTrayIcon;

/// A transparent, draggable top-level desktop-pet window.
class PetWindow final : public QWidget
{
    Q_OBJECT

public:
    enum class HorizontalBoundary { None, Left, Right };

    explicit PetWindow(QWidget *parent = nullptr);
    void setPetFrame(const QPixmap &frame);
    [[nodiscard]] HorizontalBoundary moveHorizontally(int pixels);
    void showSpeechBubble(const QString &text, int durationMs = 2500);
    void setPetScale(double scale);
    void setAlwaysOnTop(bool enabled);
    void setClickThroughEnabled(bool enabled);

signals:
    void exitRequested();
    void clicked();
    void dragStarted();
    void dragReleased();
    void chatRequested();
    void settingsRequested();
    void studyRequested();
    void statsRequested();

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
    [[nodiscard]] QRect currentWorkArea() const;
    void setClickThrough(bool enabled);

    QPixmap m_currentPixmap;
    QPoint m_dragStartGlobal;
    QPoint m_windowStart;
    bool m_pointerPressed = false;
    bool m_dragging = false;
    bool m_isQuitting = false;
    QSystemTrayIcon *m_trayIcon = nullptr;
    QMenu *m_trayMenu = nullptr;
    QAction *m_clickThroughAction = nullptr;
};

