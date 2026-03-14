#ifndef DRAWINGGUI_QGRAPHICSITEMVIEWIMAGE_H
#define DRAWINGGUI_QGRAPHICSITEMVIEWIMAGE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGIView.h"
#include "QGIUserTypes.h"

namespace TechDraw {
class DrawViewImage;
}

namespace TechDrawGui
{
class QGCustomImage;
class QGCustomClip;

class TechDrawGuiExport QGIViewImage : public QGIView
{
public:
    QGIViewImage();
    ~QGIViewImage() override;

    enum {Type = UserType::QGIViewImage};
    int type() const override { return Type;}

    void updateView(bool update = false) override;
    void setViewImageFeature(TechDraw::DrawViewImage *obj);

    void draw() override;
    void rotateView() override;

protected:
    virtual void drawImage();

    QGCustomImage* m_imageItem;
    QGCustomClip*  m_cliparea;
};

} // namespace
#endif // DRAWINGGUI_QGRAPHICSITEMVIEWIMAGE_H
