# include <cassert>

# include <QPainter>
# include <QPainterPath>
# include <QStyleOptionGraphicsItem>

#include <Base/Tools2D.h>

#include "QGIVertex.h"
#include "PreferencesGui.h"
#include "QGIPrimPath.h"


using namespace TechDrawGui;

QGIVertex::QGIVertex(int index) :
    projIndex(index),
    m_radius(2)
{
    QColor vertexColor = PreferencesGui::vertexQColor();
    setFill(vertexColor, Qt::SolidPattern);

    setRadius(m_radius);
}

void QGIVertex::setRadius(double r)
{
    m_radius = r;
    QPainterPath p;
    p.addEllipse(-r/2.0, -r/2.0, r, r);
    setPath(p);
}

void QGIVertex::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

//    painter->setPen(Qt::blue);
//    painter->drawRect(boundingRect());          //good for debugging

//    m_brush.setColor(m_colCurrent);
//    m_brush.setStyle(m_fill);
//    setBrush(m_brush);
    QGIPrimPath::paint (painter, &myOption, widget);
}

Base::Vector2d QGIVertex::toVector2d() const
{
    QPointF center = boundingRect().center();
    center = mapToScene(center);
    return Base::Vector2d(center.x(), center.y());
}

//! Returns a vector drawn from own position to p2
Base::Vector2d QGIVertex::vector2dBetweenPoints(const QGIVertex* p2) const
{
    return p2->toVector2d() - toVector2d();
}
