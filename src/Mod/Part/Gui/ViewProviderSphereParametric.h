#ifndef PARTGUI_VIEWPROVIDERSPHEREPARAMETRIC_H
#define PARTGUI_VIEWPROVIDERSPHEREPARAMETRIC_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProviderPrimitive.h"

namespace PartGui
{

class PartGuiExport ViewProviderSphereParametric: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderSphereParametric);

public:
    /// constructor
    ViewProviderSphereParametric();
    /// destructor
    ~ViewProviderSphereParametric() override;

    std::vector<std::string> getDisplayModes() const override;

protected:
};

class PartGuiExport ViewProviderEllipsoid: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderEllipsoid);

public:
    /// constructor
    ViewProviderEllipsoid();
    /// destructor
    ~ViewProviderEllipsoid() override;
    std::vector<std::string> getDisplayModes() const override;
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERSPHEREPARAMETRIC_H
