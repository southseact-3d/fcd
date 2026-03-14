#ifndef DRAWINGGUI_QGRAPHICSITEMVERTEX_H
#define DRAWINGGUI_QGRAPHICSITEMVERTEX_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGIPrimPath.h"
#include "QGIUserTypes.h"

namespace Base {
class Vector2d;
}

namespace TechDrawGui
{

class TechDrawGuiExport QGIVertex : public QGIPrimPath
{
public:
    explicit QGIVertex(int index);
    ~QGIVertex() override = default;

    enum {Type = UserType::QGIVertex};
    int type() const override { return Type;}
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr ) override;

    int getProjIndex() const { return projIndex; }

    double getRadius() const { return m_radius; }
    virtual void setRadius(double r);

    Base::Vector2d toVector2d() const;
    Base::Vector2d vector2dBetweenPoints(const QGIVertex* p2) const;

protected:
    bool multiselectEligible() override { return true; }

    int projIndex;
    double m_radius;
};

}

#endif // DRAWINGGUI_QGRAPHICSITEMVERTEX_H
