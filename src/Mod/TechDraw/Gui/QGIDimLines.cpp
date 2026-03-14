# include <cassert>

# include <QPainterPath>
# include <QPainterPathStroker>

#include "QGIDimLines.h"
#include "PreferencesGui.h"


using namespace TechDrawGui;
using namespace TechDraw;

QGIDimLines::QGIDimLines()
{
    setCacheMode(QGraphicsItem::NoCache);
    setAcceptHoverEvents(false);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setWidth(0.5);
}

void QGIDimLines::draw()
{
}

QPainterPath QGIDimLines::shape() const
{
    QPainterPath outline;
    QPainterPathStroker stroker;
    stroker.setWidth(getEdgeFuzz());
    outline = stroker.createStroke(path());
    return outline;
}

double QGIDimLines::getEdgeFuzz() const
{
    return PreferencesGui::edgeFuzz();
}


QRectF QGIDimLines::boundingRect() const
{
    return shape().controlPointRect().adjusted(-3, -3, 3, 3);
}
