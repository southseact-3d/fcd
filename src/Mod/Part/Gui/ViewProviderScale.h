#ifndef PARTGUI_VIEWPROVIDERSCALE_H
#define PARTGUI_VIEWPROVIDERSCALE_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProvider.h"


namespace PartGui
{

class PartGuiExport ViewProviderScale: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderScale);

public:
    /// constructor
    ViewProviderScale();
    /// destructor
    ~ViewProviderScale() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERSCALE_H
