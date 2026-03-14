#ifndef TECHDRAWGUI_VIEWPROVIDERVIEWSECTION_H
#define TECHDRAWGUI_VIEWPROVIDERVIEWSECTION_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <Mod/TechDraw/App/DrawViewSection.h>

#include "ViewProviderViewPart.h"


namespace TechDrawGui {

class TechDrawGuiExport ViewProviderViewSection : public ViewProviderViewPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderViewSection);

public:
    /// constructor
    ViewProviderViewSection();
    /// destructor
    ~ViewProviderViewSection() override;

    App::PropertyColor   CutSurfaceColor;
    App::PropertyPercent CutSurfaceTransparency;

    App::PropertyBool   HatchCutSurface;       //obsolete - use CutSurfaceDisplay
    App::PropertyColor  HatchColor;
    App::PropertyColor  GeomHatchColor;
    App::PropertyFloat  WeightPattern;

    void updateData(const App::Property*) override;
    void onChanged(const App::Property *prop) override;
    bool setEdit(int ModNum) override;
    bool doubleClicked() override;

    void updateGraphic();
    void getParameters();
    bool canDelete(App::DocumentObject* obj) const override;

    TechDraw::DrawViewSection* getViewObject() const override;
};

} // namespace TechDrawGui


#endif // TECHDRAWGUI_VIEWPROVIDERVIEWSECTION_H
