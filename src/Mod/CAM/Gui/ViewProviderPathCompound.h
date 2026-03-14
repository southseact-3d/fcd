#ifndef PATH_ViewProviderPathCompound_H
#define PATH_ViewProviderPathCompound_H

#include "ViewProviderPath.h"

namespace PathGui
{

class PathGuiExport ViewProviderPathCompound: public ViewProviderPath
{
    PROPERTY_HEADER_WITH_OVERRIDE(PathGui::ViewProviderPathCompound);

public:
    std::vector<App::DocumentObject*> claimChildren() const override;
    bool canDragObjects() const override;
    void dragObject(App::DocumentObject*) override;
    bool canDropObjects() const override;
    void dropObject(App::DocumentObject*) override;
    QIcon getIcon() const override;

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
};

using ViewProviderPathCompoundPython = Gui::ViewProviderFeaturePythonT<ViewProviderPathCompound>;

}  // namespace PathGui


#endif  // PATH_ViewProviderPathCompound_H
