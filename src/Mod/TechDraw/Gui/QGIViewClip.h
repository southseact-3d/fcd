#ifndef DRAWINGGUI_QGRAPHICSITEMCLIP_H
#define DRAWINGGUI_QGRAPHICSITEMCLIP_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGIView.h"
#include "QGIUserTypes.h"

namespace TechDraw
{
class DrawView;
}

namespace TechDrawGui
{
class QGCustomRect;
class QGCustomClip;

class TechDrawGuiExport QGIViewClip : public QGIView
{
public:

    QGIViewClip();
    ~QGIViewClip() override = default;

    enum {Type = UserType::QGIViewClip};
    int type() const override { return Type;}
    bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;
    TechDraw::DrawView* selectionIsInGroup() const;
    bool forwardEventToSelection(QGIView* qview, QEvent *event) const;

    void updateView(bool update = false) override;

    void draw() override;
    QGCustomRect* getFrame() {return m_frame;}
    QGCustomClip* getClipArea() {return m_cliparea;}

protected:
    void drawClip();

private:
    QGCustomRect* m_frame;
    QGCustomClip* m_cliparea;

};

} // namespace MDIViewPageGui

#endif // DRAWINGGUI_QGRAPHICSITEMCLIP_H
