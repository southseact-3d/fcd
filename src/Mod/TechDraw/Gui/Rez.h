#ifndef DRAWINGGUI_REZ_H
#define DRAWINGGUI_REZ_H

#include <QPointF>
#include <QRectF>
#include <QSize>

#include <Base/Tools2D.h>
#include <Base/Vector3D.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDrawGui
{

/// Functions to handle mm resolution conversion
class TechDrawGuiExport Rez
{
public:
    static double getParameter(void);
    static double getRezFactor(void);
    static void setRezFactor(double f);
//turn App side value to Gui side value
    static double guiX(double x);
    static Base::Vector3d guiX(Base::Vector3d v);
    static Base::Vector2d guiX(Base::Vector3d v, bool planar);
    static QPointF guiX(QPointF p);

//turn Gui side value to App side value
    static double appX(double x);
    static Base::Vector3d appX(Base::Vector3d v);
    static QPointF appX(QPointF p);

    static QPointF guiPt(QPointF p);
    static QPointF appPt(QPointF p);

    static QRectF guiRect(QRectF r);
    static QSize guiSize(QSize s);
    static QSize appSize(QSize s);

private:
    static double m_rezFactor;
};

} //end namespace TechDrawGui
#endif
