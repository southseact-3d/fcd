# include <QGuiApplication>
# include <QMouseEvent>

#include "QGVNavStyleSolidWorks.h"
#include "QGVPage.h"


using namespace TechDrawGui;

namespace TechDrawGui {

QGVNavStyleSolidWorks::QGVNavStyleSolidWorks(QGVPage* qgvp) :
    QGVNavStyle(qgvp)
{
}

QGVNavStyleSolidWorks::~QGVNavStyleSolidWorks()
{
}

void QGVNavStyleSolidWorks::handleKeyReleaseEvent(QKeyEvent *event)
{
    // zoom mode 2
    if ((event->key() == Qt::Key_Shift) && zoomingActive) {
        stopZoom();
        event->accept();
    }
}

void QGVNavStyleSolidWorks::handleMousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
//    Base::Console().message("QGVNSSolidWorks::handleMousePressEvent() - button: %d buttons: %d\n", event->button(), event->buttons());
}

void QGVNavStyleSolidWorks::handleMouseMoveEvent(QMouseEvent *event)
{

    if (getViewer()->isBalloonPlacing()) {
        balloonCursorMovement(event);
        return;
    }

    if ((QGuiApplication::mouseButtons() & Qt::MiddleButton) &&
        QGuiApplication::keyboardModifiers().testFlag(Qt::ShiftModifier) ) {
        //zoom mode 2 Shift + MMB
       if (zoomingActive) {
           zoom(mouseZoomFactor(event->pos()));
       } else {
           startZoom(event->pos());
       }
       event->accept();
    } else if ((QGuiApplication::mouseButtons() & Qt::MiddleButton) &&
        QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier) ) {
        // pan mode 1 - Ctrl + MMB
        // I think this block may be unnecessary since pan mode 2 MMB also captures pan mode 1
        // Ctrl + MMB, but I'll leave it in to make it clear that this is what's intended
        // also nav style OCC has a similar block
        if (panningActive) {
            pan(event->pos());
        } else {
            startPan(event->pos());
        }
        event->accept();
    } else if (QGuiApplication::mouseButtons() & Qt::MiddleButton) {
        // pan mode 2 - MMB
        if (panningActive) {
            pan(event->pos());
        } else {
            startPan(event->pos());
        }
        event->accept();
    }
}

void QGVNavStyleSolidWorks::handleMouseReleaseEvent(QMouseEvent *event)
{
//    Base::Console().message("QGVNSSolidWorks::handleMouseReleaseEvent() - button: %d buttons: %d\n", event->button(), event->buttons());
    if (getViewer()->isBalloonPlacing()) {
        placeBalloon(event->pos());
    }

    if (panningActive) {
        if (event->button() == Qt::MiddleButton) {
            //pan mode 1 or 2 - [Control] + MMB
            //stop panning if MMB released
            if (panningActive) {
                stopPan();
                event->accept();
            }

            //zoom mode 2 Shift + MMB
            //stop zooming if MMB released
            if (zoomingActive) {
                stopZoom();
                event->accept();
            }
        }
    }
}

}  // namespace TechDrawGui
