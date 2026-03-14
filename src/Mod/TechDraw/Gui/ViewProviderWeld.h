#ifndef DRAWINGGUI_VIEWPROVIDERWELD_H
#define DRAWINGGUI_VIEWPROVIDERWELD_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <Gui/ViewProviderFeature.h>

#include "ViewProviderDrawingView.h"
#include <Mod/TechDraw/App/DrawTile.h>
#include <Mod/TechDraw/App/DrawWeldSymbol.h>


namespace TechDrawGui {

class TechDrawGuiExport ViewProviderWeld : public ViewProviderDrawingView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderWeld);

public:
    /// constructor
    ViewProviderWeld();
    /// destructor
    ~ViewProviderWeld() override;

    App::PropertyString      Font;
    App::PropertyLength      FontSize;
    App::PropertyLength      TileFontSize;

    bool useNewSelectionModel() const override {return false;}
    void onChanged(const App::Property* p) override;
    std::vector<App::DocumentObject*> claimChildren() const override;
    bool setEdit(int ModNum) override;
    bool doubleClicked() override;

    TechDraw::DrawWeldSymbol* getViewObject() const override;
    virtual TechDraw::DrawWeldSymbol* getFeature() const;

    std::string prefFontName();
    double prefFontSize();
    double prefTileTextAdjust();
    bool onDelete(const std::vector<std::string> &) override;
    bool canDelete(App::DocumentObject* obj) const override;

};

} // namespace TechDrawGui


#endif // DRAWINGGUI_VIEWPROVIDERWELD_H
