#ifndef DRAWINGGUI_QGRAPHICSITEMVIEWANNOTATION_H
#define DRAWINGGUI_QGRAPHICSITEMVIEWANNOTATION_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGIView.h"
#include "QGIUserTypes.h"

namespace TechDraw {
class DrawViewAnnotation;
}

namespace TechDrawGui
{
class QGCustomText;

class TechDrawGuiExport QGIViewAnnotation : public QGIView
{
public:

    explicit QGIViewAnnotation();

    /// m_textItem belongs to this group and will be deleted by Qt
    ~QGIViewAnnotation() override = default;

    enum {Type = UserType::QGIViewAnnotation};
    int type() const override { return Type;}

    void updateView(bool update = false) override;
    void setViewAnnoFeature(TechDraw::DrawViewAnnotation *obj);

    void draw() override;
    void rotateView() override;

protected:
    void drawAnnotation();

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    QGCustomText *m_textItem;
    QColor m_colNormal;
    QColor m_colSel;
    QColor m_colPre;
};

} // end namespace TechDrawGui

#endif // DRAWINGGUI_QGRAPHICSITEMVIEWANNOTATION_H
