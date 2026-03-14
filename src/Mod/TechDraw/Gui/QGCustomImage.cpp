# include <QPainter>
# include <QPixmap>
# include <QRectF>
# include <QStyleOptionGraphicsItem>


#include "QGCustomImage.h"


using namespace TechDrawGui;

QGCustomImage::QGCustomImage()
{
    setCacheMode(QGraphicsItem::NoCache);
    setAcceptHoverEvents(false);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

QGCustomImage::~QGCustomImage()
{
}

void QGCustomImage::centerAt(QPointF centerPos)
{
    centerAt(centerPos.x(), centerPos.y());
}

void QGCustomImage::centerAt(double cX, double cY)
{
    QRectF br = boundingRect();
    double width = br.width() * scale();
    double height = br.height() * scale();
    double newX = cX - width/2.;
    double newY = cY - height/2.;
    setPos(newX, newY);
}

bool QGCustomImage::load(QString fileSpec)
{
    bool success = true;
    QPixmap px(fileSpec);
    m_px = px;
    prepareGeometryChange();
    setPixmap(m_px);
    return(success);
}

bool QGCustomImage::load(QPixmap map)
{
    bool success = true;
    m_px = map;
    prepareGeometryChange();
    setPixmap(m_px);
    return(success);
}

QSize QGCustomImage::imageSize()
{
    return m_px.size() * scale();
}

void QGCustomImage::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

    //painter->drawRect(boundingRect());          //good for debugging

    QGraphicsPixmapItem::paint (painter, &myOption, widget);
}

