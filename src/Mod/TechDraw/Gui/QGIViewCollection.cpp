#include "QGIViewCollection.h"


using namespace TechDrawGui;

QGIViewCollection::QGIViewCollection()
{
    setFlags(QGraphicsItem::ItemIsSelectable);

    setHandlesChildEvents(false);

    //setCacheMode(QGraphicsItem::NoCache);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
}
