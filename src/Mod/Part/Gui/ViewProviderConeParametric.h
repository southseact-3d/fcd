#ifndef PARTGUI_VIEWPROVIDERCONEPARAMETRIC_H
#define PARTGUI_VIEWPROVIDERCONEPARAMETRIC_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProviderPrimitive.h"


class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;
class SoTransform;

namespace PartGui
{


class PartGuiExport ViewProviderConeParametric: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderConeParametric);

public:
    /// constructor
    ViewProviderConeParametric();
    /// destructor
    ~ViewProviderConeParametric() override;

    std::vector<std::string> getDisplayModes() const override;

protected:
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERCONEPARAMETRIC_H
