# include <QGraphicsSceneEvent>


#include <Mod/TechDraw/App/DrawUtil.h>

#include "QGIGhostHighlight.h"
#include "PreferencesGui.h"
#include "Rez.h"


using namespace TechDrawGui;
using namespace TechDraw;

QGIGhostHighlight::QGIGhostHighlight()
{
    setInteractive(true);
    m_dragging = false;

    //make the ghost very visible
    QFont f(Preferences::labelFontQString());
    double fontSize = Preferences::labelFontSizeMM();
    setFont(f, fontSize);
    setReference("drag");
    setStyle(Qt::SolidLine);
    setColor(prefSelectColor());
    setWidth(Rez::guiX(1.0));
    setRadius(10.0);         //placeholder
}

QGIGhostHighlight::~QGIGhostHighlight()
{

}

void QGIGhostHighlight::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
//    Base::Console().message("QGIGhostHighlight::mousePress() - %X\n", this);
    if ( (event->button() == Qt::LeftButton) &&
        (flags() & QGraphicsItem::ItemIsMovable) ) {
            m_dragging = true;
            event->accept();
    }
    QGIHighlight::mousePressEvent(event);
}

void QGIGhostHighlight::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
//    Base::Console().message("QGIGhostHighlight::mouseRelease() - pos: %s scenePos: %s\n",
//                                 DrawUtil::formatVector(pos()).c_str(),
//                                 DrawUtil::formatVector(mapToScene(pos())).c_str());
    if (m_dragging) {
        m_dragging = false;
        Q_EMIT positionChange(scenePos());
        event->accept();
    }
    QGIHighlight::mouseReleaseEvent(event);
}

void QGIGhostHighlight::setInteractive(bool state)
{
    setFlag(QGraphicsItem::ItemIsSelectable, state);
    setFlag(QGraphicsItem::ItemIsMovable, state);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, state);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, state);
}

//radius should scaled, but not Rez::guix()
void QGIGhostHighlight::setRadius(double r)
{
    setBounds(-r, r, r, -r);
}

#include <Mod/TechDraw/Gui/moc_QGIGhostHighlight.cpp>
