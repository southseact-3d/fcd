#include <Mod/TechDraw/App/DrawTemplate.h>

#include "QGITemplate.h"
#include "QGSPage.h"
#include "ZVALUE.h"


using namespace TechDrawGui;

QGITemplate::QGITemplate(QGSPage *scene) : QGraphicsItemGroup(),
    pageTemplate(nullptr)
{
    setHandlesChildEvents(false);
    setCacheMode(QGraphicsItem::NoCache);
    setZValue(ZVALUE::TEMPLATE); //Template is situated in background

    scene->addItem(this);
}

QGITemplate::~QGITemplate()
{
    pageTemplate = nullptr;
}

void QGITemplate::setTemplate(TechDraw::DrawTemplate *obj)
{
    if (!obj)
        return;

    pageTemplate = obj;
}

void QGITemplate::clearContents()
{

}

void QGITemplate::updateView(bool update)
{
    Q_UNUSED(update);
    draw();
}

#include <Mod/TechDraw/Gui/moc_QGITemplate.cpp>
