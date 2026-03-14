# include <QGraphicsScene>
# include <QPainter>
# include <QStyleOptionGraphicsItem>


#include "QGMText.h"


using namespace TechDrawGui;

QGMText::QGMText() :
    m_showBox(false),
    m_prettyState("Normal")
{
    setCacheMode(QGCustomText::NoCache);
    setFlag(ItemSendsGeometryChanges, true);
//    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setFlag(ItemIsMovable, true);
    setFlag(ItemIsSelectable, true);
    setAcceptHoverEvents(true);
}

QVariant QGMText::itemChange(GraphicsItemChange change, const QVariant &value)
{
    //QPointF newPos(0.0, 0.0);
    if(change == ItemPositionHasChanged && scene()) {
        Q_EMIT dragging();
    }

    return QGCustomText::itemChange(change, value);
}

void QGMText::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    if(scene() && this == scene()->mouseGrabberItem()) {
        Q_EMIT dragFinished();
    }
    QGCustomText::mouseReleaseEvent(event);
}

void QGMText::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_EMIT hover(true);
    QGCustomText::hoverEnterEvent(event);
}

void QGMText::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_EMIT hover(false);
    QGCustomText::hoverLeaveEvent(event);
}

void QGMText::setPrettySel()
{
    m_prettyState = "Sel";
    QGCustomText::setPrettySel();
}

void QGMText::setPrettyPre()
{
    m_prettyState = "Pre";
    QGCustomText::setPrettyPre();
}

void QGMText::setPrettyNormal()
{
    m_prettyState = "Normal";
    QGCustomText::setPrettyNormal();
}

void QGMText::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

//    painter->drawRect(boundingRect());          //good for debugging
    //TODO: this should be changed to a rectItem in the parent
    if (showBox()) {
        painter->drawRect(boundingRect().adjusted(1, 1,-1, -1));
    }

    QGCustomText::paint (painter, &myOption, widget);
}

#include <Mod/TechDraw/Gui/moc_QGMText.cpp>
