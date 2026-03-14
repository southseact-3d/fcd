#ifndef DRAWINGGUI_QGCUSTOMSVG_H
#define DRAWINGGUI_QGCUSTOMSVG_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QByteArray>
#include <QGraphicsItem>
#include <QGraphicsSvgItem>
#include <QPointF>
#include <QSvgRenderer>

#include "QGIUserTypes.h"

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

namespace TechDrawGui
{

class TechDrawGuiExport QGCustomSvg : public QGraphicsSvgItem
{
public:
    explicit QGCustomSvg();
    ~QGCustomSvg() override;

    enum {Type = UserType::QGCustomSvg};
    int type() const override { return Type;}

    void paint( QPainter *painter,
                const QStyleOptionGraphicsItem *option,
                QWidget *widget = nullptr ) override;
    virtual void centerAt(QPointF centerPos);
    virtual void centerAt(double cX, double cY);
    virtual bool load(QByteArray *svgString);
    virtual bool load(QString filename);

protected:
    QSvgRenderer *m_svgRender;
};

} // namespace TechDrawGui

#endif // DRAWINGGUI_QGCUSTOMSVG_H
