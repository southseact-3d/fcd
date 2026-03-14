#ifndef DRAWINGGUI_QGRAPHICSITEMVIEWCOLLECTION_H
#define DRAWINGGUI_QGRAPHICSITEMVIEWCOLLECTION_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGIView.h"
#include "QGIUserTypes.h"

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

namespace TechDrawGui
{

class TechDrawGuiExport  QGIViewCollection : public QGIView
{
public:
    QGIViewCollection();
    ~QGIViewCollection() override = default;

    enum {Type = UserType::QGIViewCollection};
    int type() const override { return Type;}
};

} // namespace MDIViewPageGui

#endif // DRAWINGGUI_QGRAPHICSITEMVIEWCOLLECTION_H
