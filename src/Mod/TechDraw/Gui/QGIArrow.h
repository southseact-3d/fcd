#ifndef DRAWINGGUI_QGRAPHICSITEMARROW_H
#define DRAWINGGUI_QGRAPHICSITEMARROW_H

#include <Mod/TechDraw/TechDrawGlobal.h>
#include <Mod/TechDraw/App/ArrowPropEnum.h>

#include <Base/Vector3D.h>

#include "QGIPrimPath.h"
#include "QGIUserTypes.h"

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

namespace TechDrawGui
{

class TechDrawGuiExport QGIArrow : public QGIPrimPath
{
public:
    explicit QGIArrow();
    ~QGIArrow() override {}

    enum {Type = UserType::QGIArrow};
    int type() const override { return Type;}

public:
    void draw();
    bool isFlipped() { return m_flipped; }
    void setFlipped(bool flipped) { m_flipped = flipped; }
    void flip() { m_flipped = !m_flipped; }
    double getSize() { return m_size; }
    void setSize(double s);
    TechDraw::ArrowType getStyle() { return m_style; }
    void setStyle(TechDraw::ArrowType s) { m_style = s; }
    bool getDirMode() { return m_dirMode; }
    void setDirMode(bool b) { m_dirMode = b; }
    Base::Vector3d getDirection(void) { return m_flipped ? -m_dir : m_dir; }
    void setDirection(Base::Vector3d v) { m_dir = v; }
    void setDirection(double angle) { m_dir = Base::Vector3d(cos(angle), sin(angle), 0.0); }
    static TechDraw::ArrowType getPrefArrowStyle();
    static double getPrefArrowSize();
    static double getOverlapAdjust(TechDraw::ArrowType style, double size);

protected:
    QPainterPath makeFilledTriangle(double length, double width, bool flipped);
    QPainterPath makeFilledTriangle(Base::Vector3d dir, double length, double width);
    QPainterPath makeOpenArrow(double length, double width, bool flipped);
    QPainterPath makeOpenArrow(Base::Vector3d dir, double length, double width);
    QPainterPath makeHashMark(double length, double width, bool flipped);
    QPainterPath makeHashMark(Base::Vector3d dir, double length, double width);
    QPainterPath makeDot(double length, double width, bool flipped);
    QPainterPath makeOpenDot(double length, double width, bool flipped);
    QPainterPath makeForkArrow(double length, double width, bool flipped);
    QPainterPath makeForkArrow(Base::Vector3d dir, double length, double width);
    QPainterPath makePyramid(double length, bool flipped);
    QPainterPath makePyramid(Base::Vector3d dir, double length);

private:
    QBrush m_brush;
    Qt::BrushStyle m_fill;
    double m_size;
    TechDraw::ArrowType m_style;
    bool m_flipped;
    bool m_dirMode;
    Base::Vector3d m_dir;
};

}

#endif // DRAWINGGUI_QGRAPHICSITEMARROW_H
