# include <QApplication>
# include <QGuiApplication>
# include <QMouseEvent>


#include "QGVNavStyleMaya.h"
#include "QGVPage.h"


using namespace TechDrawGui;

namespace TechDrawGui {

QGVNavStyleMaya::QGVNavStyleMaya(QGVPage *qgvp) :
    QGVNavStyle(qgvp)
{
}

QGVNavStyleMaya::~QGVNavStyleMaya()
{
}

void QGVNavStyleMaya::handleKeyReleaseEvent(QKeyEvent *event)
{
    //zoom mode 2
    if ( (event->key() == Qt::Key_Alt) && zoomingActive) {
        zoomingActive = false;
        event->accept();
    }

    //pan mode
    if ((event->key() == Qt::Key_Alt) && panningActive) {
        stopPan();
        event->accept();
    }
}
void QGVNavStyleMaya::handleMousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void QGVNavStyleMaya::handleMouseMoveEvent(QMouseEvent *event)
{
    if (getViewer()->isBalloonPlacing()) {
        balloonCursorMovement(event);
        return;
    }

    //pan mode alt + MMB + mouse movement
    if (QGuiApplication::mouseButtons() & Qt::MiddleButton &&
        QApplication::keyboardModifiers().testFlag(Qt::AltModifier)) {
        if (panningActive) {
            pan(event->pos());
        } else {
            startPan(event->pos());
        }
        event->accept();
    }

    //zoom mode 2 ALT + RMB
    if (QGuiApplication::mouseButtons() & Qt::RightButton &&
        QApplication::keyboardModifiers().testFlag(Qt::AltModifier)) {
        if (zoomingActive) {
            zoom(mouseZoomFactor(event->pos()));
        } else {
            startZoom(event->pos());
        }
        event->accept();
    }
}

void QGVNavStyleMaya::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (getViewer()->isBalloonPlacing()) {
        placeBalloon(event->pos());
    }

    if (event->button() == Qt::MiddleButton) {
        //pan mode ALT + MMB
        if (panningActive) {
            stopPan();
            event->accept();
        }
    }

    if (event->button() == Qt::RightButton) {
        //zoom mode 2 ALT + RMB
        if (zoomingActive) {
            zoomingActive = false;
            event->accept();
        }
    }
}

bool QGVNavStyleMaya::allowContextMenu(QContextMenuEvent *event)
{
//    Base::Console().message("QGVNSM::allowContextMenu()\n");
    if (event->reason() == QContextMenuEvent::Mouse) {
        //must check for a button combination involving context menu button
        if (QApplication::keyboardModifiers() == Qt::AltModifier) {
            //Alt is down, so this is Alt + RMB - don't allow context menu
            return false;
        }
    }
    return true;
}
}  // namespace TechDrawGui
