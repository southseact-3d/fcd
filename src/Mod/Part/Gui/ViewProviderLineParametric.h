#ifndef PARTGUI_VIEWPROVIDERLINEPARAMETRIC_H
#define PARTGUI_VIEWPROVIDERLINEPARAMETRIC_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProviderPrimitive.h"


class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;
class SoTransform;

namespace PartGui
{


class PartGuiExport ViewProviderLineParametric: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderLineParametric);

public:
    /// constructor
    ViewProviderLineParametric();
    /// destructor
    ~ViewProviderLineParametric() override;

    std::vector<std::string> getDisplayModes() const override;

protected:
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERLINEPARAMETRIC_H
