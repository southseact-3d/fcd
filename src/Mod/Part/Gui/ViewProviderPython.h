#ifndef PARTGUI_VIEWPROVIDERPYTHON_H
#define PARTGUI_VIEWPROVIDERPYTHON_H

#include <Gui/ViewProviderFeaturePython.h>
#include <Mod/Part/Gui/ViewProvider.h>

#include <Mod/Part/PartGlobal.h>

namespace PartGui
{

class PartGuiExport ViewProviderCustom: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderCustom);

public:
    /// constructor
    ViewProviderCustom();
    /// destructor
    ~ViewProviderCustom() override;
    void updateData(const App::Property*) override;

protected:
    void onChanged(const App::Property* prop) override;
    std::map<const App::Property*, Gui::ViewProvider*> propView;
};

using ViewProviderPython = Gui::ViewProviderFeaturePythonT<ViewProviderPart>;
using ViewProviderCustomPython = Gui::ViewProviderFeaturePythonT<ViewProviderCustom>;

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERPYTHON_H
