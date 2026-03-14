#ifndef TECHDRAW_NAVIGATIONSTYLE_H
#define TECHDRAW_NAVIGATIONSTYLE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QCursor>
#include <QPoint>

class QEvent;
class QFocusEvent;
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
class QContextMenuEvent;
#include <Base/BaseClass.h>

namespace TechDrawGui {

class QGVPage;

//class to support multiple key combinations
class KeyCombination
{
public:
    KeyCombination();
    ~KeyCombination();

    void addKey(int inKey);
    void removeKey(int inKey);
    void clear();
    bool empty();

    bool haveCombination(int inCombo);

private:
    std::vector<int> keys;
};

class TechDrawGuiExport QGVNavStyle : public Base::BaseClass
{
public:
    explicit QGVNavStyle(QGVPage* qgvp);
    ~QGVNavStyle() override;

    void setViewer(QGVPage* qgvp) { m_viewer = qgvp;} ;
    QGVPage* getViewer() { return m_viewer;};

    virtual void handleEnterEvent(QEvent *event);
    virtual void handleFocusOutEvent(QFocusEvent *event);
    virtual void handleKeyPressEvent(QKeyEvent *event);
    virtual void handleKeyReleaseEvent(QKeyEvent *event);
    virtual void handleLeaveEvent(QEvent *event);
    virtual void handleMouseMoveEvent(QMouseEvent *event);
    virtual void handleMousePressEvent(QMouseEvent *event);
    virtual void handleMouseReleaseEvent(QMouseEvent *event);
    virtual void handleWheelEvent(QWheelEvent *event);

    virtual bool allowContextMenu(QContextMenuEvent *event);
    virtual void pseudoContextEvent();

    virtual void startZoom(QPoint p);
    virtual void zoom(double factor);
    virtual void stopZoom();
    virtual double mouseZoomFactor(QPoint p);
    virtual void zoomIn();
    virtual void zoomOut();

    virtual void startPan(QPoint p);
    virtual void pan(QPoint p);
    virtual void stopPan();

    virtual void startClick(Qt::MouseButton b);
    virtual void stopClick();

    virtual void placeBalloon(QPoint p);
    virtual void balloonCursorMovement(QMouseEvent *event);

protected:
    virtual void initialize();
    virtual void setAnchor();

    QGVPage* m_viewer;
    int m_currentmode;

    bool ctrldown, shiftdown, altdown;
    bool button1down, button2down, button3down;
    bool invertZoom;
    bool zoomAtCursor;
    double zoomStep;
    int m_reversePan;
    int m_reverseScroll;
    QPoint panOrigin;
    bool panningActive;
    QPoint zoomOrigin;
    bool zoomingActive;
    bool m_clickPending;
    bool m_panPending;
    bool m_zoomPending;
    Qt::MouseButton m_clickButton;

    KeyCombination m_keyCombo;
    QCursor m_saveCursor;
    int m_wheelDeltaCounter;
    int m_mouseDeltaCounter;

private:

};

}
#endif // TECHDRAW_NAVIGATIONSTYLE_H
