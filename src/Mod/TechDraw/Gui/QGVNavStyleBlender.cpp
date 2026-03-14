# include <QGuiApplication>
# include <QMouseEvent>


#include "QGVNavStyleBlender.h"
#include "QGVPage.h"


using namespace TechDrawGui;

namespace TechDrawGui {

QGVNavStyleBlender::QGVNavStyleBlender(QGVPage* qgvp) :
    QGVNavStyle(qgvp)
{
}

QGVNavStyleBlender::~QGVNavStyleBlender()
{
}

void QGVNavStyleBlender::handleKeyReleaseEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Shift) && panningActive) {
        stopPan();
        event->accept();
    }
}

void QGVNavStyleBlender::handleMousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
//    Base::Console().message("QGVNSBlender::handleMousePressEvent() - button: %d buttons: %d\n", event->button(), event->buttons());
}

void QGVNavStyleBlender::handleMouseMoveEvent(QMouseEvent *event)
{
//    Base::Console().message("QGVNSBlender::handleMouseMoveEvent() - buttons: %d modifiers: %X\n",
//                            QGuiApplication::mouseButtons() & Qt::MiddleButton,
//                            QGuiApplication::keyboardModifiers().testFlag(Qt::ShiftModifier));

    if (getViewer()->isBalloonPlacing()) {
        balloonCursorMovement(event);
        return;
    }

    if ((QGuiApplication::mouseButtons() & Qt::LeftButton) &&
        (QGuiApplication::mouseButtons() & Qt::RightButton)) {
        //pan mode 1 - LMB + RMB
        if (panningActive) {
            pan(event->pos());
        } else {
            startPan(event->pos());
        }
        event->accept();
    } else if ((QGuiApplication::mouseButtons() & Qt::MiddleButton) &&
               (QGuiApplication::keyboardModifiers().testFlag(Qt::ShiftModifier)) ) {
        //pan mode 2 - Shift + MMB
        if (panningActive) {
            pan(event->pos());
        } else {
            startPan(event->pos());
        }
        event->accept();
    }
}

void QGVNavStyleBlender::handleMouseReleaseEvent(QMouseEvent *event)
{
//    Base::Console().message("QGVNSBlender::handleMouseReleaseEvent() - button: %d buttons: %d\n", event->button(), event->buttons());
    if (getViewer()->isBalloonPlacing()) {
        placeBalloon(event->pos());
    }

    if (panningActive) {
        //pan mode 1 - LMB + RMB + mouse move
        //stop panning if either button released
        if ( (event->button() == Qt::LeftButton) ||
             (event->button() == Qt::RightButton)) {
            stopPan();
            event->accept();
        }
        //pan mode 2 - Shift + MMB
        //stop panning if MMB released
        if (event->button() == Qt::MiddleButton) {
            stopPan();
            event->accept();
        }
    }
}

bool QGVNavStyleBlender::allowContextMenu(QContextMenuEvent *event)
{
//    Base::Console().message("QGVNSBlender::allowContextMenu()\n");
    if (event->reason() == QContextMenuEvent::Mouse) {
        //must check for a button combination involving context menu button
        if (QGuiApplication::mouseButtons() & Qt::LeftButton) {
            //LeftButton is down, so this is LMB + RMB - don't allow context menu
            return false;
        }
    }
    return true;
}

}  // namespace TechDrawGui
