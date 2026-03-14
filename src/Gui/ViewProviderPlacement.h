#ifndef GUI_ViewProviderPlacement_H
#define GUI_ViewProviderPlacement_H

#include "AxisOrigin.h"
#include "ViewProviderGeometryObject.h"
#include "ViewProviderFeaturePython.h"


class SoFontStyle;
class SoText2;
class SoBaseColor;
class SoTranslation;
class SoCoordinate3;
class SoIndexedLineSet;
class SoEventCallback;
class SoMaterial;

namespace Gui
{

class GuiExport ViewProviderPlacement: public ViewProviderGeometryObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderPlacement);

public:
    /// Constructor
    ViewProviderPlacement();
    ~ViewProviderPlacement() override;

    void attach(App::DocumentObject*) override;
    void updateData(const App::Property*) override;
    std::vector<std::string> getDisplayModes() const override;
    void setDisplayMode(const char* ModeName) override;

    /// indicates if the ViewProvider use the new Selection model
    bool useNewSelectionModel() const override
    {
        return true;
    }
    /// indicates if the ViewProvider can be selected
    bool isSelectable() const override;

    bool getElementPicked(const SoPickedPoint* pp, std::string& subname) const override;
    bool getDetailPath(const char*, SoFullPath*, bool, SoDetail*&) const override;

protected:
    void onChanged(const App::Property* prop) override;
};

using ViewProviderPlacementPython = ViewProviderFeaturePythonT<ViewProviderPlacement>;

}  // namespace Gui


#endif  // GUI_ViewProviderPlacement_H
