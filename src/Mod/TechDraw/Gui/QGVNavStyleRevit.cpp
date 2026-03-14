# include <QGuiApplication>
# include <QMouseEvent>


#include "QGVNavStyleRevit.h"
#include "QGVPage.h"


using namespace TechDrawGui;

namespace TechDrawGui {

QGVNavStyleRevit::QGVNavStyleRevit(QGVPage *qgvp) :
    QGVNavStyle(qgvp)
{
}

QGVNavStyleRevit::~QGVNavStyleRevit()
{
}

void QGVNavStyleRevit::handleMousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        startClick(Qt::RightButton);
    }
}

void QGVNavStyleRevit::handleMouseMoveEvent(QMouseEvent *event)
{
    if (getViewer()->isBalloonPlacing()) {
        balloonCursorMovement(event);
        return;
    }

    //if the mouse moves between press and release, then it isn't a click
    if (m_clickPending) {
        stopClick();
        return;
    }

    //pan mode 1 - MMB + move
    if (QGuiApplication::mouseButtons() & Qt::MiddleButton) {
        if (panningActive) {
            pan(event->pos());
        } else {
            startPan(event->pos());
        }
        event->accept();
    }

    //pan mode 2 - LMB + RMB + move
    if (QGuiApplication::mouseButtons() & Qt::LeftButton &&
        QGuiApplication::mouseButtons() & Qt::RightButton) {
        if (panningActive) {
            pan(event->pos());
        } else {
            startPan(event->pos());
        }
        event->accept();
    }
}

void QGVNavStyleRevit::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (getViewer()->isBalloonPlacing()) {
        placeBalloon(event->pos());
    }

    if ((event->button() == Qt::RightButton) &&
         m_clickPending &&
        (m_clickButton == Qt::RightButton)) {
        stopClick();
        pseudoContextEvent();
        event->accept();
        return;
    }

    //stop panning if any button released
    if ( (event->button() == Qt::LeftButton) ||
         (event->button() == Qt::RightButton) ||
         (event->button() == Qt::MiddleButton) ){
        if (panningActive) {
            stopPan();
            event->accept();
        }
    }
}

bool QGVNavStyleRevit::allowContextMenu(QContextMenuEvent *event)
{
//    Base::Console().message("QGVNSRevit::allowContextMenu()\n");
    if (event->reason() == QContextMenuEvent::Mouse) {
        //must check for a button combination involving context menu button
        if (QGuiApplication::mouseButtons() & Qt::LeftButton) {
            //LMB down - don't allow context menu
            return false;
        } else if (m_clickPending) {
            //context menu request to be handled by button release
            return false;
        }
    }
    return true;
}
}  // namespace TechDrawGui
