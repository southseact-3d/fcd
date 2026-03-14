#ifndef PARTGUI_ViewProviderMainPart_H
#define PARTGUI_ViewProviderMainPart_H

#include <Mod/Part/Gui/ViewProvider.h>


namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderMainPart: public PartGui::ViewProviderPart
{
    PROPERTY_HEADER(PartDesignGui::ViewProviderMainPart);

public:
    /// constructor
    ViewProviderMainPart();
    /// destructor
    virtual ~ViewProviderMainPart();

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren(void) const;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderMainPart_H
