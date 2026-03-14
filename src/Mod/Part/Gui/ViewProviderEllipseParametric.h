#ifndef PARTGUI_VIEWPROVIDERELLIPSEPARAMETRIC_H
#define PARTGUI_VIEWPROVIDERELLIPSEPARAMETRIC_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProviderPrimitive.h"


class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;
class SoTransform;

namespace PartGui
{


class PartGuiExport ViewProviderEllipseParametric: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderEllipseParametric);

public:
    /// constructor
    ViewProviderEllipseParametric();
    /// destructor
    ~ViewProviderEllipseParametric() override;

    std::vector<std::string> getDisplayModes() const override;

protected:
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERELLIPSEPARAMETRIC_H
