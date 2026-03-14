# include <QPainterPath>


#include <Mod/TechDraw/App/Geometry.h>
#include <Mod/TechDraw/App/DrawParametricTemplate.h>

#include "QGIDrawingTemplate.h"


using namespace TechDrawGui;

QGIDrawingTemplate::QGIDrawingTemplate(QGSPage* scene) : QGITemplate(scene),
                                                                                    pathItem(nullptr)
{
    pathItem = new QGraphicsPathItem;

    // Invert the Y for the QGraphicsPathItem with Y pointing upwards
    QTransform qtrans;
    qtrans.scale(1., -1.);

    pathItem->setTransform(qtrans);

    addToGroup(pathItem);
}

QGIDrawingTemplate::~QGIDrawingTemplate()
{
    pathItem = nullptr;
}

void QGIDrawingTemplate::clearContents()
{

}

TechDraw::DrawParametricTemplate * QGIDrawingTemplate::getParametricTemplate()
{
    if(pageTemplate && pageTemplate->isDerivedFrom<TechDraw::DrawParametricTemplate>())
        return static_cast<TechDraw::DrawParametricTemplate *>(pageTemplate);
    else
        return nullptr;
}

void QGIDrawingTemplate::draw()
{

    TechDraw::DrawParametricTemplate *tmplte = getParametricTemplate();
    if(!tmplte) {
        throw Base::RuntimeError("Template Feuature not set for QGIDrawingTemplate");
    }


    // Clear the previous geometry stored

    // Get a list of geometry and iterate
    const TechDraw::BaseGeomPtrVector &geoms =  tmplte->getGeometry();

    TechDraw::BaseGeomPtrVector::const_iterator it = geoms.begin();

    QPainterPath path;

    // Draw Edges
    // iterate through all the geometries
    for(; it != geoms.end(); ++it) {
        if((*it)->getGeomType() == TechDraw::GeomType::GENERIC) {
            TechDraw::GenericPtr geom = std::static_pointer_cast<TechDraw::Generic>(*it);

            path.moveTo(geom->points[0].x, geom->points[0].y);
            std::vector<Base::Vector3d>::const_iterator it = geom->points.begin();

            for(++it; it != geom->points.end(); ++it) {
                path.lineTo((*it).x, (*it).y);
            }
        }
    }

    pathItem->setPath(path);
}

void QGIDrawingTemplate::updateView(bool update)
{
    Q_UNUSED(update);
    draw();
}

#include <Mod/TechDraw/Gui/moc_QGIDrawingTemplate.cpp>
