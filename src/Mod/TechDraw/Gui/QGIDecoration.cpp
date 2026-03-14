# include <cassert>

# include <QPainter>
# include <QStyleOptionGraphicsItem>

#include "QGIDecoration.h"
#include "QGICMark.h"
#include "PreferencesGui.h"
#include "ZVALUE.h"


using namespace TechDrawGui;
using namespace TechDraw;

QGIDecoration::QGIDecoration() :
    m_pen(Qt::SolidLine),
    m_dragState(DragState::NoDrag)
{
    setCacheMode(QGraphicsItem::NoCache);
    setAcceptHoverEvents(false);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    setWidth(1.0);
}

void QGIDecoration::draw()
{
}

void QGIDecoration::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

    // painter->setPen(Qt::green);
    // painter->drawRect(boundingRect());          //good for debugging

    QGraphicsItemGroup::paint (painter, &myOption, widget);
}

void QGIDecoration::setWidth(double w)
{
    m_pen.setWidthF(w);
}

void QGIDecoration::setStyle(Qt::PenStyle s)
{
    m_pen.setStyle(s);
}

void QGIDecoration::setColor(QColor c)
{
    m_pen.setColor(c);
    m_brush.setColor(c);
}

QColor QGIDecoration::prefNormalColor()
{
    return PreferencesGui::normalQColor();
}

QColor QGIDecoration::prefPreColor()
{
    return PreferencesGui::preselectQColor();
}

QColor QGIDecoration::prefSelectColor()
{
    return PreferencesGui::selectQColor();
}

QRectF QGIDecoration::boundingRect() const
{
    return childrenBoundingRect();
}


void QGIDecoration::makeMark(double x, double y)
{
    QGICMark* cmItem = new QGICMark(-1);
    cmItem->setParentItem(this);
    cmItem->setPos(x, y);
    cmItem->setThick(2.0);
    cmItem->setSize(40.0);
    cmItem->setZValue(ZVALUE::VERTEX);
}

void QGIDecoration::makeMark(Base::Vector3d v)
{
    makeMark(v.x, v.y);
}

void QGIDecoration::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
//    Base::Console().message("QGID::mousePressEvent() - %s\n", getViewName());
    m_dragState = DragState::DragStarted;

    QGraphicsItem::mousePressEvent(event);
}

void QGIDecoration::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    if (m_dragState == DragState::DragStarted) {
        m_dragState = DragState::Dragging;
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void QGIDecoration::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
//    Base::Console().message("QGID::mouseReleaseEvent() - %s\n", getViewName());
    if (m_dragState == DragState::Dragging) {
        onDragFinished();
    }
    m_dragState = DragState::NoDrag;

    QGraphicsItem::mouseReleaseEvent(event);
}

void QGIDecoration::onDragFinished()
{
    //override this
}
