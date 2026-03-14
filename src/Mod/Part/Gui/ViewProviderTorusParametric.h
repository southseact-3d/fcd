#ifndef PARTGUI_VIEWPROVIDERTORUSPARAMETRIC_H
#define PARTGUI_VIEWPROVIDERTORUSPARAMETRIC_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProviderPrimitive.h"


class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;
class SoTransform;

namespace PartGui
{


class PartGuiExport ViewProviderTorusParametric: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderTorusParametric);

public:
    /// constructor
    ViewProviderTorusParametric();
    /// destructor
    ~ViewProviderTorusParametric() override;

    std::vector<std::string> getDisplayModes() const override;

protected:
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERTORUSPARAMETRIC_H
