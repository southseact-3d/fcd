#ifndef PARTGUI_VIEWPROVIDERCOMPOUND_H
#define PARTGUI_VIEWPROVIDERCOMPOUND_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProvider.h"


namespace PartGui
{

class PartGuiExport ViewProviderCompound: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderCompound);

public:
    /// constructor
    ViewProviderCompound();
    /// destructor
    ~ViewProviderCompound() override;
    std::vector<App::DocumentObject*> claimChildren() const override;
    bool onDelete(const std::vector<std::string>&) override;

    /// drag and drop
    bool canDragObjects() const override;
    bool canDragObject(App::DocumentObject*) const override;
    void dragObject(App::DocumentObject*) override;
    bool canDropObjects() const override;
    bool canDropObject(App::DocumentObject*) const override;
    void dropObject(App::DocumentObject*) override;

protected:
    void updateData(const App::Property*) override;
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERCOMPOUND_H
