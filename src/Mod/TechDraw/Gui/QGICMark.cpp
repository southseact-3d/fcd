# include <cassert>

# include <QPainter>
# include <QPainterPath>
# include <QStyleOptionGraphicsItem>

#include <App/Material.h>
#include <Base/Parameter.h>
#include <Mod/TechDraw/App/Preferences.h>

#include "QGICMark.h"
#include "PreferencesGui.h"

using namespace TechDraw;
using namespace TechDrawGui;

QGICMark::QGICMark(int index) : QGIVertex(index)
{
    m_size = 3.0;
    setThick(0.75);
    draw();
}
void QGICMark::draw()
{
    QPainterPath cmPath;
    cmPath.moveTo(0.0, m_size);
    cmPath.lineTo(0.0, -m_size);
    cmPath.moveTo(m_size, 0.0);
    cmPath.lineTo(-m_size, 0.0);
    setPath(cmPath);
}

void QGICMark::setSize(float s)
{
    m_size = s;
    draw();
}

void QGICMark::setThick(float t)
{
    m_pen.setWidthF(t);
    draw();
}

QColor QGICMark::getCMarkColor()
{
    return PreferencesGui::centerQColor();
}

void QGICMark::setPrettyNormal() {
    m_pen.setColor(getCMarkColor());
    update();
}

void QGICMark::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

    QGIVertex::paint (painter, &myOption, widget);
}

QRectF QGICMark::boundingRect() const
{
    return shape().controlPointRect();
}

QPainterPath QGICMark::shape() const
{
    QPainterPath outline;
    QPainterPathStroker stroker;
    stroker.setWidth(getMarkFuzz());
    outline = stroker.createStroke(path());
    return outline;
}

 double QGICMark::getMarkFuzz() const
{
    return Preferences::getPreferenceGroup("General")->GetFloat("MarkFuzz", 5.0);
}

