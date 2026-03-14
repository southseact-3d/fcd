# include <cmath>

# include <QPainter>
# include <QPainterPath>
# include <QStyleOptionGraphicsItem>

#include <Base/Parameter.h>
#include <Mod/TechDraw/App/Preferences.h>

#include "QGICenterLine.h"
#include "PreferencesGui.h"


using namespace TechDraw;
using namespace TechDrawGui;

QGICenterLine::QGICenterLine()
{
    m_line = new QGraphicsPathItem();
    addToGroup(m_line);
    setWidth(0.0);
    setStyle(getCenterStyle());
    setColor(getCenterColor());
    m_isintersection = false;              //Coverity CID 174669
}

void QGICenterLine::draw()
{
    prepareGeometryChange();
    makeLine();
    update();
}

void QGICenterLine::makeLine()
{
    QPainterPath pp;
    pp.moveTo(m_start);
    pp.lineTo(m_end);
    m_line->setPath(pp);
}


void QGICenterLine::setBounds(double x1, double y1, double x2, double y2)
{
    m_start = QPointF(x1, y1);
    m_end = QPointF(x2, y2);
}

QColor QGICenterLine::getCenterColor()
{
    return PreferencesGui::centerQColor();
}

Qt::PenStyle QGICenterLine::getCenterStyle()
{
    Qt::PenStyle centerStyle = static_cast<Qt::PenStyle> (Preferences::getPreferenceGroup("Decorations")->GetInt("CenterLine", 2));
    return centerStyle;
}

void QGICenterLine::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

    setTools();
    QGIDecoration::paint (painter, &myOption, widget);
}

void QGICenterLine::setIntersection(bool isIntersecting) {
    /**
     * Set the intersection style for the centerline.
     * If isIntersecting is set to true, the middle of the centerline
     * will be the middle of a dash - therefore if two lines intersect, they
     * will form a cross.
     * If isIntersecting is set to false, the middle of the centerline will be a
     * dot.
     */
    m_isintersection = isIntersecting;
}

void QGICenterLine::setTools()
{
    if (m_pen.style() == Qt::DashDotLine) {
        QVector<qreal> dashes;
        qreal space = 4;  // in unit width
        qreal dash = 16;
        // dot must be really small when using CapStyle RoundCap but > 0
        // for CapStyle FlatCap you would need to set it to 1
        qreal dot = 0.000001;

        dashes << dot << space << dash << space;
        qreal dashlen = dot + 2 * space + dash;
        qreal l_len = sqrt(pow(m_start.x() - m_end.x(), 2) + pow(m_start.y() - m_end.y(), 2)) / 2.0;
        // convert from pixelunits to width units
        l_len = l_len / m_pen.widthF();
        // note that the additional length using RoundCap or SquareCap does not
        // count here!
        if (m_isintersection) {
            m_pen.setDashOffset(dashlen - fmod(l_len, dashlen) + space + dash / 2);
        } else {
            m_pen.setDashOffset(dashlen - fmod(l_len, dashlen));
        }

        m_pen.setDashPattern(dashes);
    }
    m_pen.setCapStyle(Qt::RoundCap);
    m_line->setPen(m_pen);
}

void QGICenterLine::setLinePen(QPen isoPen)
{
    m_pen = isoPen;
}
