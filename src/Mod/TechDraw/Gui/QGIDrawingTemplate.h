#ifndef DRAWINGGUI_QGRAPHICSITEMDRAWINGTEMPLATE_H
#define DRAWINGGUI_QGRAPHICSITEMDRAWINGTEMPLATE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGITemplate.h"
#include "QGIUserTypes.h"

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsPathItem;
QT_END_NAMESPACE

namespace TechDraw {
class DrawParametricTemplate;
}

namespace TechDrawGui
{
class QGSPage;

class TechDrawGuiExport  QGIDrawingTemplate : public QGITemplate
{
    Q_OBJECT

public:
    explicit QGIDrawingTemplate(QGSPage *);
    ~QGIDrawingTemplate() override;

    enum {Type = UserType::QGIDrawingTemplate};
    int type() const override { return Type;}

    void clearContents();
    void draw() override;
    void updateView(bool update = false) override;

protected:
  TechDraw::DrawParametricTemplate * getParametricTemplate();

protected:
  QGraphicsPathItem *pathItem;
};

} // namespace MDIViewPageGui

#endif // DRAWINGGUI_QGRAPHICSITEMDRAWINGTEMPLATE_H
