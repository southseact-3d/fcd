# include <QGuiApplication>
# include <QMouseEvent>


#include "QGVNavStyleInventor.h"
#include "QGVPage.h"


using namespace TechDrawGui;

namespace TechDrawGui {

//**********
// Issue: select should be Shift + LMB
//        currently is just LMB like all the other styles
//        need to just accept LMB w/o Shift and pass Shift + LMB to
//        QGraphicsView?
//**********

QGVNavStyleInventor::QGVNavStyleInventor(QGVPage *qgvp) :
    QGVNavStyle(qgvp)
{
}

QGVNavStyleInventor::~QGVNavStyleInventor()
{
}

void QGVNavStyleInventor::handleMousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void QGVNavStyleInventor::handleMouseMoveEvent(QMouseEvent *event)
{
    if (getViewer()->isBalloonPlacing()) {
        balloonCursorMovement(event);
        return;
    }

    if ((QGuiApplication::mouseButtons() & Qt::LeftButton) &&
        (QGuiApplication::mouseButtons() & Qt::MiddleButton)) {
        //zoom mode 2 - LMB + MMB
        if (zoomingActive) {
            zoom(mouseZoomFactor(event->pos()));
        } else {
            startZoom(event->pos());
        }
        event->accept();
    } else if (QGuiApplication::mouseButtons() & Qt::MiddleButton)  {
        //pan mode - MMB + move
        if (panningActive) {
            pan(event->pos());
        } else {
            startPan(event->pos());
        }
        event->accept();
    }
}

void QGVNavStyleInventor::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (getViewer()->isBalloonPlacing()) {
        placeBalloon(event->pos());
    }

    if (event->button() == Qt::MiddleButton) {
        //pan mode MMB
        if (panningActive) {
            stopPan();
            event->accept();
        }
    }

    if ((event->button() == Qt::LeftButton) ||
        (event->button() == Qt::MiddleButton) ){
        //zoom mode 2 LMB + MMB
        if (zoomingActive) {
            zoomingActive = false;
            event->accept();
        }
    }
}

}  // namespace TechDrawGui
