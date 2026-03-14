#ifndef DRAWINGGUI_QGIProjGroup_H
#define DRAWINGGUI_QGIProjGroup_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGIViewCollection.h"
#include "QGIUserTypes.h"


QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QEvent;
QT_END_NAMESPACE

namespace TechDraw {
    class DrawProjGroup;
}

namespace TechDrawGui
{
class QGIViewPart;

class TechDrawGuiExport QGIProjGroup : public QGIViewCollection
{
public:
    QGIProjGroup();

    // TODO: if the QGIVO is deleted, should we clean up any remaining QGIVParts??
    ~QGIProjGroup() override = default;

    enum {Type = UserType::QGIProjGroup};
    int type() const override { return Type;}

    void alignTo(QGIProjGroup *, const QString &alignment);

    void rotateView() override;

    void drawBorder() override;

    bool isMember(App::DocumentObject* dvpObj) const;
    QGIView* getAnchorQItem() const;
    TechDraw::DrawProjGroup* getPGroupFeature() const;
    QList<QGIViewPart*> secondaryQViews() const;

protected:
    bool sceneEventFilter(QGraphicsItem* watched, QEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    // Mouse handling
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event ) override;
    void mousePressEvent(QGraphicsSceneMouseEvent * event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event) override;

    void mouseReleaseEvent(QGIView* originator, QGraphicsSceneMouseEvent* event);

private:
    /// Convenience function
    bool autoDistributeEnabled() const;

    QGraphicsItem* m_origin;
    QPoint mousePos;
};

} // namespace MDIViewPageGui

#endif // DRAWINGGUI_QGIProjGroup_H
