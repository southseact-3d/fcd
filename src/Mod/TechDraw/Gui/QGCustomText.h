#ifndef DRAWINGGUI_QGCUSTOMTEXT_H
#define DRAWINGGUI_QGCUSTOMTEXT_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QPointF>

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

#include <Base/Parameter.h>
#include <Base/Vector3D.h>

#include "QGIUserTypes.h"

namespace TechDrawGui
{

class TechDrawGuiExport QGCustomText : public QGraphicsTextItem
{
public:
    explicit QGCustomText(QGraphicsItem* parent = nullptr);
    ~QGCustomText() override {}

    enum {Type = UserType::QGCustomText};
    int type() const override { return Type;}
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr ) override;

    QRectF tightBoundingRect() const;
    QRectF alignmentRect() const;
    QPointF tightBoundingAdjust() const;


    void setHighlighted(bool state);
    virtual void setPrettyNormal();
    virtual void setPrettyPre();
    virtual void setPrettySel();

    virtual void centerAt(QPointF centerPos);
    virtual void centerAt(double cX, double cY);
    virtual void justifyLeftAt(QPointF centerPos, bool vCenter = true);
    virtual void justifyLeftAt(double cX, double cY, bool vCenter = true);
    virtual void justifyRightAt(QPointF centerPos, bool vCenter = true);
    virtual void justifyRightAt(double cX, double cY, bool vCenter = true);

    virtual double getHeight();
    virtual double getWidth();

    virtual QColor getNormalColor();
    virtual QColor getPreColor();
    virtual QColor getSelectColor();
    virtual void setColor(QColor c);

    virtual void setTightBounding(bool tight);

    void makeMark(double x, double y);
    void makeMark(Base::Vector3d v);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    Base::Reference<ParameterGrp> getParmGroup();

    bool tightBounding;  // Option to use tighter boundingRect(), works only for plaintext QGCustomText
    QColor m_colNormal;

private:

};

}

#endif // DRAWINGGUI_QGCUSTOMTEXT_H
