#ifndef DRAWINGGUI_QGIMATTING_H
#define DRAWINGGUI_QGIMATTING_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsPathItem>
#include <QBrush>
#include <QPen>

#include "QGIUserTypes.h"


QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

namespace TechDrawGui
{
class QGCustomRect;

class TechDrawGuiExport QGIMatting : public QGraphicsItemGroup
{
public:
    explicit QGIMatting();
    ~QGIMatting() override {}

    enum {Type = UserType::QGIMatting};
    int type() const override { return Type;}

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr ) override;
    QRectF boundingRect() const override;

    virtual void setSize(double w, double h) {m_height = h; m_width = w;}
    //virtual void setHoleStyle(int hs) {m_holeStyle = hs;}
    virtual void setRadius(double r)  {m_radius = r;}
    virtual void draw();

protected:
    double m_height;
    double m_width;
    double m_radius;
    double m_fudge;
    int getHoleStyle();

    QGraphicsPathItem* m_border;
    QGraphicsPathItem* m_mat;

private:
    QPen m_pen;
    QBrush m_brush;
    QPen m_matPen;
    QBrush m_matBrush;

};

} // namespace MDIViewPageGui

#endif // DRAWINGGUI_QGIMATTING_H
