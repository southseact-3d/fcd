#ifndef DRAWINGGUI_QGIDECORATION_H
#define DRAWINGGUI_QGIDECORATION_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QBrush>
#include <QGraphicsItemGroup>
#include <QPen>


QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

#include <Base/Parameter.h>
#include <Base/Console.h>
#include <Base/Vector3D.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "Enums.h"
#include "QGIUserTypes.h"

namespace TechDrawGui
{

class TechDrawGuiExport QGIDecoration : public QGraphicsItemGroup
{
public:
    explicit QGIDecoration();
    ~QGIDecoration() override = default;
    enum {Type = UserType::QGIDecoration};
    int type() const override { return Type;}

    QRectF boundingRect() const override;
    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget = nullptr ) override;
    virtual void draw();

    // Mouse handling
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void onDragFinished();

    void setWidth(double w);
    double getWidth() { return m_pen.widthF(); }
    void setStyle(Qt::PenStyle s);
    void setColor(QColor c);
    QColor getColor() { return m_colNormal; }
    void makeMark(double x, double y);
    void makeMark(Base::Vector3d v);

protected:
    void setPrettyNormal();
    void setPrettyPre();
    void setPrettySel();
    virtual QColor prefNormalColor();
    virtual QColor prefPreColor();
    virtual QColor prefSelectColor();
    QPen m_pen;
    QBrush m_brush;
    QColor m_colNormal;

    DragState m_dragState;

private:
};

}
#endif
