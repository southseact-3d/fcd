#ifndef PARTGUI_VIEWPROVIDEREXTRUSION_H
#define PARTGUI_VIEWPROVIDEREXTRUSION_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProvider.h"


namespace PartGui
{

class PartGuiExport ViewProviderExtrusion: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderExtrusion);

public:
    /// constructor
    ViewProviderExtrusion();
    /// destructor
    ~ViewProviderExtrusion() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDEREXTRUSION_H
