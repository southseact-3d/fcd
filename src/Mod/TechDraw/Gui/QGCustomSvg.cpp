# include <QPainter>
# include <QRectF>
# include <QStyleOptionGraphicsItem>


#include "QGCustomSvg.h"


using namespace TechDrawGui;

QGCustomSvg::QGCustomSvg()
{
    setCacheMode(QGraphicsItem::NoCache);
    setAcceptHoverEvents(false);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    m_svgRender = new QSvgRenderer();
}

QGCustomSvg::~QGCustomSvg()
{
    delete m_svgRender;
}

void QGCustomSvg::centerAt(QPointF centerPos)
{
    centerAt(centerPos.x(), centerPos.y());
}

void QGCustomSvg::centerAt(double cX, double cY)
{
    QRectF box = boundingRect();
    double width = box.width();
    double height = box.height();
    double newX = (cX - width/2.) * scale();
    double newY = (cY - height/2.) * scale();
    setPos(newX, newY);
}

bool QGCustomSvg::load(QByteArray *svgBytes)
{
    bool success = m_svgRender->load(*svgBytes);
    prepareGeometryChange();
    setSharedRenderer(m_svgRender);
    return(success);
}

bool QGCustomSvg::load(QString filename)
{
    bool success = m_svgRender->load(filename);
    prepareGeometryChange();
    setSharedRenderer(m_svgRender);
    return(success);
}

void QGCustomSvg::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

//    painter->setPen(Qt::yellow);
//    painter->drawRect(boundingRect());          //good for debugging

    QGraphicsSvgItem::paint (painter, &myOption, widget);
}
