#ifndef PATH_ViewProviderPathShape_H
#define PATH_ViewProviderPathShape_H

#include "ViewProviderPath.h"

namespace PathGui
{

class PathGuiExport ViewProviderPathShape: public ViewProviderPath
{
    PROPERTY_HEADER_WITH_OVERRIDE(PathGui::ViewProviderPathShape);

public:
    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
    void updateData(const App::Property*) override;
    bool onDelete(const std::vector<std::string>&) override;

    /// drag and drop
    bool canDragObjects() const override;
    bool canDragObject(App::DocumentObject*) const override;
    void dragObject(App::DocumentObject*) override;
    bool canDropObjects() const override;
    bool canDropObject(App::DocumentObject*) const override;
    void dropObject(App::DocumentObject*) override;

    QIcon getIcon() const override;
};

}  // namespace PathGui


#endif  // PATH_ViewProviderPathShape_H
