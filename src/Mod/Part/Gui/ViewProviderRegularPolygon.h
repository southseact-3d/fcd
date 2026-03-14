#ifndef PARTGUI_VIEWPROVIDERREGULARPOLYGON_H
#define PARTGUI_VIEWPROVIDERREGULARPOLYGON_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProviderPrimitive.h"


namespace PartGui
{


class PartGuiExport ViewProviderRegularPolygon: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderRegularPolygon);

public:
    /// constructor
    ViewProviderRegularPolygon();
    /// destructor
    ~ViewProviderRegularPolygon() override;

    std::vector<std::string> getDisplayModes() const override;

protected:
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERREGULARPOLYGON_H
