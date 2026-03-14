#ifndef DRAWINGGUI_QGCUSTOMCLIP_H
#define DRAWINGGUI_QGCUSTOMCLIP_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QGraphicsItem>
#include <QPointF>
#include <QRectF>

#include <Base/Vector3D.h>

#include "QGIUserTypes.h"

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

namespace TechDrawGui
{

class TechDrawGuiExport QGCustomClip : public QGraphicsItemGroup
{
public:
    explicit QGCustomClip();
    ~QGCustomClip() override {}

    enum {Type = UserType::QGCustomClip};
    int type() const override { return Type;}
    QRectF boundingRect() const override;

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr ) override;
    virtual void centerAt(QPointF centerPos);
    virtual void centerAt(double cX, double cY);
    virtual void setRect(QRectF r);
    virtual void setRect(double x, double y, double w, double h);
    virtual QRectF rect();
    void makeMark(double x, double y);
    void makeMark(Base::Vector3d v);

protected:

private:
    QRectF m_rect;

};

} // namespace MDIViewPageGui

#endif // DRAWINGGUI_QGCUSTOMCLIP_H

