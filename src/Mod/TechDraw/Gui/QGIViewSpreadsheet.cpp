#include <Mod/TechDraw/App/DrawViewSpreadsheet.h>

#include "QGIViewSpreadsheet.h"


using namespace TechDrawGui;

QGIViewSpreadsheet::QGIViewSpreadsheet()
{
    setHandlesChildEvents(false);
    setCacheMode(QGraphicsItem::NoCache);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}


void QGIViewSpreadsheet::setViewFeature(TechDraw::DrawViewSpreadsheet *obj)
{
    // called from QGVPage. (once)
    QGIView::setViewFeature(static_cast<TechDraw::DrawView *>(obj));
}

