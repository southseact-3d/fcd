#ifndef PARTGUI_VIEWPROVIDERCYLINDERPARAMETRIC_H
#define PARTGUI_VIEWPROVIDERCYLINDERPARAMETRIC_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProviderPrimitive.h"


class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;
class SoTransform;

namespace PartGui
{


class PartGuiExport ViewProviderCylinderParametric: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderCylinderParametric);

public:
    /// constructor
    ViewProviderCylinderParametric();
    /// destructor
    ~ViewProviderCylinderParametric() override;

    std::vector<std::string> getDisplayModes() const override;

protected:
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERCYLINDERPARAMETRIC_H
