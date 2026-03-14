#ifndef PARTGUI_VIEWPROVIDERPRISM_H
#define PARTGUI_VIEWPROVIDERPRISM_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProviderPrimitive.h"


namespace PartGui
{


class PartGuiExport ViewProviderPrism: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderPrism);

public:
    /// constructor
    ViewProviderPrism();
    /// destructor
    ~ViewProviderPrism() override;

    std::vector<std::string> getDisplayModes() const override;

protected:
};

class PartGuiExport ViewProviderWedge: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderWedge);

public:
    /// constructor
    ViewProviderWedge();
    /// destructor
    ~ViewProviderWedge() override;

    std::vector<std::string> getDisplayModes() const override;

protected:
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERPRISM_H
