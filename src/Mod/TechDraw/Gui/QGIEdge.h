#ifndef DRAWINGGUI_QGRAPHICSITEMEDGE_H
#define DRAWINGGUI_QGRAPHICSITEMEDGE_H

#include <Mod/TechDraw/App/Geometry.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGIPrimPath.h"
#include "QGIUserTypes.h"

namespace TechDrawGui
{

class TechDrawGuiExport QGIEdge : public QGIPrimPath
{
public:
    explicit QGIEdge(int index);
    ~QGIEdge() override = default;

    enum {Type = UserType::QGIEdge};

    int type() const override { return Type;}
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    int getProjIndex() const { return projIndex; }

    void setCosmetic(bool state);
    void setHiddenEdge(bool b);
    bool getHiddenEdge() const { return(isHiddenEdge); }
    void setSmoothEdge(bool b) { isSmoothEdge = b; }
    bool getSmoothEdge() const { return(isSmoothEdge); }
    void setPrettyNormal() override;

    double getEdgeFuzz() const;

    void setLinePen(const QPen& isoPen);

    void setSource(TechDraw::SourceType source) { m_source = source; }
    TechDraw::SourceType getSource() const { return m_source;}

protected:

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    bool multiselectEligible() override { return true; }

    QColor getHiddenColor();

private:
    int projIndex;                                                     //index of edge in Projection. must exist.

    bool isCosmetic;
    bool isHiddenEdge;
    bool isSmoothEdge;

    TechDraw::SourceType m_source{TechDraw::SourceType::GEOMETRY};
};

}

#endif // DRAWINGGUI_QGRAPHICSITEMEDGE_H
