#ifndef DRAWINGGUI_QGCUSTOMLABEL_H
#define DRAWINGGUI_QGCUSTOMLABEL_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QGraphicsItem>
#include <QGraphicsTextItem>

#include "QGIUserTypes.h"


QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

namespace TechDrawGui
{

class TechDrawGuiExport QGCustomLabel : public QGraphicsTextItem
{
public:
    explicit QGCustomLabel();
    ~QGCustomLabel() override = default;

    enum {Type = UserType::QGCustomLabel};
    int type() const override { return Type;}

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr ) override;
    virtual void centerAt(QPointF centerPos);
    virtual void centerAt(double cX, double cY);

protected:

private:

};

}

#endif // DRAWINGGUI_QGCUSTOMLABEL_H
