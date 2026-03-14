#ifndef TECHDRAWGUI_QGICENTERLINE_H
#define TECHDRAWGUI_QGICENTERLINE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QColor>
#include <QPointF>

#include "QGIDecoration.h"
#include "QGIUserTypes.h"

namespace TechDrawGui
{

class TechDrawGuiExport QGICenterLine : public QGIDecoration
{
public:
    explicit QGICenterLine();
    ~QGICenterLine() override = default;

    enum {Type = UserType::QGICenterLine};
    int type() const override { return Type;}

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr ) override;

    void setBounds(double x1, double y1, double x2, double y2);
    void draw() override;

    void setIntersection(bool isIntersecting);

    void setLinePen(QPen isoPen);

protected:
    QColor getCenterColor();
    Qt::PenStyle getCenterStyle();
    void makeLine();
    void setTools();

private:
    QGraphicsPathItem* m_line;           //primpath?
    QPointF            m_start;
    QPointF            m_end;
    bool               m_isintersection;
};

}

#endif // TECHDRAWGUI_QGICENTERLINE_H
