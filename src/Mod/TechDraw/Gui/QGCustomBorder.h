#ifndef DRAWINGGUI_QGCUSTOMBORDER_H
#define DRAWINGGUI_QGCUSTOMBORDER_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QPointF>

#include "QGIUserTypes.h"

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

namespace TechDrawGui
{

class TechDrawGuiExport QGCustomBorder : public QGraphicsRectItem
{
public:
    explicit QGCustomBorder();
    ~QGCustomBorder() override = default;

    enum {Type = UserType::QGCustomBorder};
    int type() const override { return Type;}

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr ) override;
    virtual void centerAt(QPointF centerPos);
    virtual void centerAt(double cX, double cY);

protected:

private:

};

} // namespace MDIViewPageGui

#endif // DRAWINGGUI_QGCUSTOMBORDER_H
