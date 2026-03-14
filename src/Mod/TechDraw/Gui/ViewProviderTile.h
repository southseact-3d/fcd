#ifndef DRAWINGGUI_VIEWPROVIDERTILE_H
#define DRAWINGGUI_VIEWPROVIDERTILE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <Gui/ViewProviderDocumentObject.h>
#include <Mod/TechDraw/App/DrawTile.h>

namespace TechDrawGui {

class TechDrawGuiExport ViewProviderTile : public Gui::ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderTile);

public:
    /// constructor
    ViewProviderTile();
    /// destructor
    ~ViewProviderTile() override;

    bool useNewSelectionModel() const override {return false;}
    bool canDelete(App::DocumentObject *obj) const override;

/*    virtual TechDraw::DrawTile* getViewObject() const;*/
    virtual TechDraw::DrawTile* getFeature() const;
};

} // namespace TechDrawGui


#endif // DRAWINGGUI_VIEWPROVIDERTILE_H
