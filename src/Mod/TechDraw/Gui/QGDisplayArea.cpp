# include <cassert>

# include <QPainter>
# include <QStyleOptionGraphicsItem>

#include "QGDisplayArea.h"


using namespace TechDrawGui;

QGDisplayArea::QGDisplayArea()
{
    setHandlesChildEvents(false);
    setCacheMode(QGraphicsItem::NoCache);
    setAcceptHoverEvents(false);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

void QGDisplayArea::centerAt(QPointF centerPos)
{
    centerAt(centerPos.x(), centerPos.y());
}

void QGDisplayArea::centerAt(double cX, double cY)
{
    QRectF box = boundingRect();
    double width = box.width();
    double height = box.height();
    double newX = cX - width/2.;
    double newY = cY - height/2.;
    setPos(newX, newY);
}

void QGDisplayArea::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

    //painter->drawRect(boundingRect());          //good for debugging

    QGraphicsItemGroup::paint (painter, &myOption, widget);
}

QRectF QGDisplayArea::boundingRect() const
{
    return childrenBoundingRect();
}

