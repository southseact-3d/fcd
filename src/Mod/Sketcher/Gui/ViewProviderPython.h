#ifndef SketcherGui_VIEWPROVIDERPYTHON_H
#define SketcherGui_VIEWPROVIDERPYTHON_H

#include <Gui/ViewProviderFeaturePython.h>
#include <Mod/Sketcher/Gui/ViewProviderSketch.h>


namespace SketcherGui
{

class SketcherGuiExport ViewProviderCustom: public ViewProviderSketch
{
    PROPERTY_HEADER_WITH_OVERRIDE(SketcherGui::ViewProviderCustom);

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

using ViewProviderPython = Gui::ViewProviderFeaturePythonT<ViewProviderSketch>;
using ViewProviderCustomPython = Gui::ViewProviderFeaturePythonT<ViewProviderCustom>;

}  // namespace SketcherGui


#endif  // SketcherGui_VIEWPROVIDERPYTHON_H
