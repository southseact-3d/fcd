#ifndef PARTGUI_VIEWPROVIDERBOX_H
#define PARTGUI_VIEWPROVIDERBOX_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProviderPrimitive.h"


class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;
class SoTransform;

namespace PartGui
{


class PartGuiExport ViewProviderBox: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderBox);

public:
    /// constructor
    ViewProviderBox();
    /// destructor
    ~ViewProviderBox() override;

    std::vector<std::string> getDisplayModes() const override;

protected:
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERBOX_H
