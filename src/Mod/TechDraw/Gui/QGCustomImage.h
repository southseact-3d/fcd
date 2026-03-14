#ifndef DRAWINGGUI_QGCUSTOMIMAGE_H
#define DRAWINGGUI_QGCUSTOMIMAGE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QPointF>
#include <QSize>

#include "QGIUserTypes.h"

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

namespace TechDrawGui
{

class TechDrawGuiExport QGCustomImage : public QGraphicsPixmapItem
{
public:
    explicit QGCustomImage();
    ~QGCustomImage() override;

    enum {Type = UserType::QGCustomImage};
    int type() const override { return Type;}

    void paint( QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget = nullptr ) override;
    virtual void centerAt(QPointF centerPos);
    virtual void centerAt(double cX, double cY);
    virtual bool load(QString fileSpec);
    virtual bool load(QPixmap map);
    virtual QSize imageSize();

protected:
    QPixmap m_px;

};

} // namespace TechDrawGui

#endif // DRAWINGGUI_QGCUSTOMIMAGE_H
