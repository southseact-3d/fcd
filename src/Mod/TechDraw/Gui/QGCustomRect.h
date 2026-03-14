#ifndef DRAWINGGUI_QGCUSTOMRECT_H
#define DRAWINGGUI_QGCUSTOMRECT_H

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

class TechDrawGuiExport QGCustomRect : public QGraphicsRectItem
{
public:
    explicit QGCustomRect();
    ~QGCustomRect() override = default;

    enum {Type = UserType::QGCustomRect};
    int type() const override { return Type;}

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr ) override;
    virtual void centerAt(QPointF centerPos);
    virtual void centerAt(double cX, double cY);

protected:

private:

};

} // namespace MDIViewPageGui

#endif // DRAWINGGUI_QGCUSTOMRECT_H

