# include <QGuiApplication>
# include <QMouseEvent>


#include "QGVNavStyleGesture.h"
#include "QGVPage.h"


using namespace TechDrawGui;

namespace TechDrawGui {

QGVNavStyleGesture::QGVNavStyleGesture(QGVPage* qgvp) :
    QGVNavStyle(qgvp)
{
}

QGVNavStyleGesture::~QGVNavStyleGesture()
{
}

void QGVNavStyleGesture::handleMousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        startClick(Qt::RightButton);
    }
}

void QGVNavStyleGesture::handleMouseMoveEvent(QMouseEvent *event)
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

    if (QGuiApplication::mouseButtons() & Qt::RightButton) {
        //pan mode 1 - RMB + move
        if (panningActive) {
            pan(event->pos());
        } else {
            startPan(event->pos());
        }
        event->accept();
    }
}

void QGVNavStyleGesture::handleMouseReleaseEvent(QMouseEvent *event)
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

    if (event->button() == Qt::RightButton) {
        stopPan();
        event->accept();
    }
}

//RMB for pan conflicts with RMB for context menu
bool QGVNavStyleGesture::allowContextMenu(QContextMenuEvent *event)
{
    if (event->reason() == QContextMenuEvent::Mouse) {
        return false;
    }
    return true;
}

}  // namespace TechDrawGui
