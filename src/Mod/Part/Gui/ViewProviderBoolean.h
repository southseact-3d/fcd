#ifndef PARTGUI_ViewProviderBoolean_H
#define PARTGUI_ViewProviderBoolean_H

#include <Mod/Part/PartGlobal.h>

#include "ViewProvider.h"


namespace PartGui
{

class PartGuiExport ViewProviderBoolean: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderBoolean);

public:
    /// constructor
    ViewProviderBoolean();
    /// destructor
    ~ViewProviderBoolean() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
    QIcon getIcon() const override;
    void updateData(const App::Property*) override;
    bool onDelete(const std::vector<std::string>&) override;
};

/// ViewProvider for the MultiFuse feature
class PartGuiExport ViewProviderMultiFuse: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderMultiFuse);

public:
    /// constructor
    ViewProviderMultiFuse();
    /// destructor
    ~ViewProviderMultiFuse() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
    QIcon getIcon() const override;
    void updateData(const App::Property*) override;
    bool onDelete(const std::vector<std::string>&) override;

    /// drag and drop
    bool canDragObjects() const override;
    bool canDragObject(App::DocumentObject*) const override;
    void dragObject(App::DocumentObject*) override;
    bool canDropObjects() const override;
    bool canDropObject(App::DocumentObject*) const override;
    void dropObject(App::DocumentObject*) override;
};

/// ViewProvider for the MultiFuse feature
class PartGuiExport ViewProviderMultiCommon: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderMultiCommon);

public:
    /// constructor
    ViewProviderMultiCommon();
    /// destructor
    ~ViewProviderMultiCommon() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
    QIcon getIcon() const override;
    void updateData(const App::Property*) override;
    bool onDelete(const std::vector<std::string>&) override;

    /// drag and drop
    bool canDragObjects() const override;
    bool canDragObject(App::DocumentObject*) const override;
    void dragObject(App::DocumentObject*) override;
    bool canDropObjects() const override;
    bool canDropObject(App::DocumentObject*) const override;
    void dropObject(App::DocumentObject*) override;
};


}  // namespace PartGui


#endif  // PARTGUI_ViewProviderBoolean_H
