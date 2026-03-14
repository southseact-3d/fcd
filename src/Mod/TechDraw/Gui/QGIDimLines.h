#ifndef DRAWINGGUI_QGRAPHICSITEMDIMLINES_H
#define DRAWINGGUI_QGRAPHICSITEMDIMLINES_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGIPrimPath.h"
#include "QGIUserTypes.h"

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

namespace TechDrawGui
{

class TechDrawGuiExport QGIDimLines : public QGIPrimPath
{
public:
    explicit QGIDimLines();
    ~QGIDimLines() override = default;

    enum {Type = UserType::QGIDimLines};
    int type() const override { return Type;}
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

public:
    void draw();
    //void setHighlighted(bool state);
    //double getLineWidth() { return m_lineWidth; }
    //void setLineWidth(double w);
    //QPainterPath shape() const;

protected:
    //QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    double getEdgeFuzz() const;


private:
};

}

#endif // DRAWINGGUI_QGRAPHICSITEMDIMLINES_H
