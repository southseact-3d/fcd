# include <cassert>

# include <QPainter>
# include <QRectF>
# include <QStyleOptionGraphicsItem>

#include "QGCustomLabel.h"


using namespace TechDrawGui;

QGCustomLabel::QGCustomLabel()
{
    setCacheMode(QGraphicsItem::NoCache);
    setAcceptHoverEvents(false);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
}

void QGCustomLabel::centerAt(QPointF centerPos)
{
    centerAt(centerPos.x(), centerPos.y());
}

void QGCustomLabel::centerAt(double cX, double cY)
{
    QRectF box = boundingRect();
    double width = box.width();
    double height = box.height();
    double newX = cX - width/2.;
    double newY = cY - height/2.;
    setPos(newX, newY);
}

void QGCustomLabel::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

//    painter->drawRect(boundingRect());          //good for debugging

    QGraphicsTextItem::paint (painter, &myOption, widget);
}
