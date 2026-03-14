#ifndef PARTGUI_VIEWPROVIDERRULEDSURFACE_H
#define PARTGUI_VIEWPROVIDERRULEDSURFACE_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProvider.h"


namespace PartGui
{


class PartGuiExport ViewProviderRuledSurface: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderRuledSurface);

public:
    /// constructor
    ViewProviderRuledSurface();
    /// destructor
    ~ViewProviderRuledSurface() override;

    std::vector<std::string> getDisplayModes() const override;
    std::vector<App::DocumentObject*> claimChildren() const override;
    void updateData(const App::Property*) override;
    bool onDelete(const std::vector<std::string>&) override;

protected:
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERRULEDSURFACE_H
