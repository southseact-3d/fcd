#ifndef DRAWINGGUI_QGRAPHICSITEMVIEWSECTION_H
#define DRAWINGGUI_QGRAPHICSITEMVIEWSECTION_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGIViewPart.h"
#include "QGIUserTypes.h"

namespace TechDrawGui
{

class TechDrawGuiExport QGIViewSection : public QGIViewPart
{
public:

    QGIViewSection() = default;
    ~QGIViewSection() override = default;

    void draw() override;
    void updateView(bool update = false) override;
    enum {Type = UserType::QGIViewSection};
    int type() const override { return Type; }

protected:
    void drawSectionFace();
};

} // end namespace TechDrawGui

#endif // #ifndef DRAWINGGUI_QGRAPHICSITEMVIEWSECTION_H
