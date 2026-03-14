#ifndef PARTGUI_VIEWPROVIDERCIRCLEPARAMETRIC_H
#define PARTGUI_VIEWPROVIDERCIRCLEPARAMETRIC_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProviderPrimitive.h"


class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;
class SoTransform;

namespace PartGui
{


class PartGuiExport ViewProviderCircleParametric: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderCircleParametric);

public:
    /// constructor
    ViewProviderCircleParametric();
    /// destructor
    ~ViewProviderCircleParametric() override;

    std::vector<std::string> getDisplayModes() const override;

protected:
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERCIRCLEPARAMETRIC_H
