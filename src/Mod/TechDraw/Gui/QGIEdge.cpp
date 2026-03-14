# include <QPainterPath>
# include <QPainterPathStroker>


#include <App/Application.h>
#include <App/Material.h>
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <Gui/Control.h>
#include <Mod/TechDraw/App/DrawUtil.h>

#include "QGIEdge.h"
#include "PreferencesGui.h"
#include "TaskLineDecor.h"
#include "QGIView.h"

using namespace TechDrawGui;
using namespace TechDraw;

QGIEdge::QGIEdge(int index) :
    projIndex(index),
    isCosmetic(false),
    isHiddenEdge(false),
    isSmoothEdge(false)
{
    setFlag(QGraphicsItem::ItemIsFocusable, true);      // to get key press events
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    setWidth(1.0);
    setCosmetic(isCosmetic);
    setFill(Qt::NoBrush);
}

// NOTE this refers to Qt cosmetic lines (a line with minimum width),
// not FreeCAD cosmetic lines
void QGIEdge::setCosmetic(bool state)
{
    isCosmetic = state;
    if (state) {
        setWidth(0.0);
    }
}

void QGIEdge::setHiddenEdge(bool b) {
    isHiddenEdge = b;
}

void QGIEdge::setPrettyNormal() {
    if (isHiddenEdge) {
        m_pen.setColor(getHiddenColor());
        return;
    }
    QGIPrimPath::setPrettyNormal();
}

QColor QGIEdge::getHiddenColor()
{
    Base::Color fcColor = Base::Color((uint32_t) Preferences::getPreferenceGroup("Colors")->GetUnsigned("HiddenColor", 0x000000FF));
    return PreferencesGui::getAccessibleQColor(fcColor.asValue<QColor>());
}


 double QGIEdge::getEdgeFuzz() const
{
    return PreferencesGui::edgeFuzz();
}


QRectF QGIEdge::boundingRect() const
{
    return shape().controlPointRect();
}

QPainterPath QGIEdge::shape() const
{
    QPainterPath outline;
    QPainterPathStroker stroker;
    stroker.setWidth(getEdgeFuzz());
    outline = stroker.createStroke(path());
    return outline;
}

void QGIEdge::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    auto* parent = dynamic_cast<QGIView *>(parentItem());
    if (parent && parent->getViewObject() && parent->getViewObject()->isDerivedFrom<TechDraw::DrawViewPart>()) {
        auto* baseFeat = static_cast<TechDraw::DrawViewPart *>(parent->getViewObject());
        std::vector<std::string> edgeName(1, DrawUtil::makeGeomName("Edge", getProjIndex()));

        Gui::Control().showDialog(new TaskDlgLineDecor(baseFeat, edgeName));
    }
}

void QGIEdge::setLinePen(const QPen& linePen)
{
    m_pen = linePen;
}

