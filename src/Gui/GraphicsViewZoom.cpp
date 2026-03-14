#include <QApplication>
#include <QGraphicsView>
#include <QMouseEvent>
#include <qmath.h>


#include <GraphicsViewZoom.h>

GraphicsViewZoom::GraphicsViewZoom(QGraphicsView* view)
    : QObject(view)
    , _view(view)
    , m_invert_zoom(false)
{
    _view->viewport()->installEventFilter(this);
    _view->setMouseTracking(true);
    _modifiers = Qt::ControlModifier;
    _zoom_factor_base = 1.0015;
}

void GraphicsViewZoom::gentle_zoom(double factor)
{
    _view->scale(factor, factor);
    _view->centerOn(target_scene_pos);
    QPointF delta_viewport_pos = target_viewport_pos
        - QPointF(_view->viewport()->width() / 2.0, _view->viewport()->height() / 2.0);
    QPointF viewport_center = _view->mapFromScene(target_scene_pos) - delta_viewport_pos;
    _view->centerOn(_view->mapToScene(viewport_center.toPoint()));
}

void GraphicsViewZoom::set_modifiers(Qt::KeyboardModifiers modifiers)
{
    _modifiers = modifiers;
}

void GraphicsViewZoom::set_zoom_factor_base(double value)
{
    _zoom_factor_base = value;
}

bool GraphicsViewZoom::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseMove) {
        auto mouse_event = static_cast<QMouseEvent*>(event);
        QPointF delta = target_viewport_pos - mouse_event->pos();
        if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5) {
            target_viewport_pos = mouse_event->pos();
            target_scene_pos = _view->mapToScene(mouse_event->pos());
        }
    }
    else if (event->type() == QEvent::Wheel) {
        auto wheel_event = static_cast<QWheelEvent*>(event);
        if (QApplication::keyboardModifiers() == _modifiers) {
            QPoint delta = wheel_event->angleDelta();
            if (qAbs(delta.y()) > qAbs(delta.x())) {  // vertical
                double angle = -delta.y();
                if (m_invert_zoom) {
                    angle = -angle;
                }
                double factor = qPow(_zoom_factor_base, angle);
                gentle_zoom(factor);
                return true;
            }
        }
    }
    Q_UNUSED(object);
    return false;
}

#include "moc_GraphicsViewZoom.cpp"
