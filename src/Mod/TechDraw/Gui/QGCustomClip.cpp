# include <cassert>

# include <QPainter>
# include <QStyleOptionGraphicsItem>

#include "QGCustomClip.h"
#include "QGICMark.h"
#include "ZVALUE.h"


using namespace TechDrawGui;

QGCustomClip::QGCustomClip()
{
    setHandlesChildEvents(false);                //not sure if needs to handle events for Views in Group???
    setCacheMode(QGraphicsItem::NoCache);
    setAcceptHoverEvents(false);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
//    setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);   //good for debugging
    m_rect = QRectF(0., 0., 10., 10.);
}

void QGCustomClip::centerAt(QPointF centerPos)
{
    centerAt(centerPos.x(), centerPos.y());
}

void QGCustomClip::centerAt(double cX, double cY)
{
    QRectF box = boundingRect();
    double width = box.width();
    double height = box.height();
    double newX = cX - width/2.;
    double newY = cY - height/2.;
    setPos(newX, newY);
}

void QGCustomClip::setRect(QRectF r)
{
    //prepareGeometryChange();??
    m_rect = r;
}

void QGCustomClip::setRect(double x, double y, double w, double h)
{
    QRectF r(x, y,w, h);
    setRect(r);
}

QRectF QGCustomClip::rect()
{
    return m_rect;
}

void QGCustomClip::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

//    painter->drawRect(boundingRect());          //good for debugging

    QGraphicsItemGroup::paint (painter, &myOption, widget);
}

QRectF QGCustomClip::boundingRect() const     //sb shape()?
{
    return m_rect;
}

void QGCustomClip::makeMark(double x, double y)
{
    QGICMark* cmItem = new QGICMark(-1);
    cmItem->setParentItem(this);
    cmItem->setPos(x, y);
    cmItem->setThick(1.0);
    cmItem->setSize(40.0);
    cmItem->setZValue(ZVALUE::VERTEX);
}

void QGCustomClip::makeMark(Base::Vector3d v)
{
    makeMark(v.x, v.y);
}


