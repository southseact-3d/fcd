#ifndef DRAWINGGUI_QGRAPHICSITEMVIEWSPREADSHEET_H
#define DRAWINGGUI_QGRAPHICSITEMVIEWSPREADSHEET_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGIViewSymbol.h"
#include "QGIUserTypes.h"

namespace TechDraw {
class DrawViewSpreadsheet;
}

namespace TechDrawGui
{

class TechDrawGuiExport QGIViewSpreadsheet : public QGIViewSymbol
{
public:
    QGIViewSpreadsheet();
    ~QGIViewSpreadsheet() override = default;

    enum {Type = UserType::QGISpreadsheet};
    int type() const override { return Type;}

    void setViewFeature(TechDraw::DrawViewSpreadsheet *obj);

};

} // end namespace TechDrawGui

#endif // DRAWINGGUI_QGRAPHICSITEMVIEWSPREADSHEET_H
