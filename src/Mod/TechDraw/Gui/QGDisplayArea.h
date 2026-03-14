#ifndef DRAWINGGUI_QGDISPLAYAREA_H
#define DRAWINGGUI_QGDISPLAYAREA_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QGraphicsItem>
#include <QPointF>
#include <QRectF>

#include "QGIUserTypes.h"

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

namespace TechDrawGui
{

class TechDrawGuiExport QGDisplayArea : public QGraphicsItemGroup
{
public:
    explicit QGDisplayArea();
    ~QGDisplayArea() override {}

    enum {Type = UserType::QGDisplayArea};
    int type() const override { return Type;}
    QRectF boundingRect() const override;

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr ) override;
    virtual void centerAt(QPointF centerPos);
    virtual void centerAt(double cX, double cY);

protected:

private:

};

}

#endif // DRAWINGGUI_QGDISPLAYAREA_H

