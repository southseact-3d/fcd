# include <QGuiApplication>
# include <QMouseEvent>


#include "QGVNavStyleTinkerCAD.h"
#include "QGVPage.h"


using namespace TechDrawGui;

namespace TechDrawGui {

QGVNavStyleTinkerCAD::QGVNavStyleTinkerCAD(QGVPage *qgvp) :
    QGVNavStyle(qgvp)
{
}

QGVNavStyleTinkerCAD::~QGVNavStyleTinkerCAD()
{
}

void QGVNavStyleTinkerCAD::handleMousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void QGVNavStyleTinkerCAD::handleMouseMoveEvent(QMouseEvent *event)
{
    if (getViewer()->isBalloonPlacing()) {
        balloonCursorMovement(event);
        return;
    }

    //pan mode - MMB + move
    if (QGuiApplication::mouseButtons() & Qt::MiddleButton) {
        if (panningActive) {
            pan(event->pos());
        } else {
            startPan(event->pos());
        }
        event->accept();
    }
}

void QGVNavStyleTinkerCAD::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (getViewer()->isBalloonPlacing()) {
        placeBalloon(event->pos());
    }

    if (event->button() == Qt::MiddleButton) {
        if (panningActive) {
            stopPan();
            event->accept();
        }
    }
}

}  // namespace TechDrawGui
