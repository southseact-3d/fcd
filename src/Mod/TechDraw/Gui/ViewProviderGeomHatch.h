#ifndef DRAWINGGUI_VIEWPROVIDERCROSSHATCH_H
#define DRAWINGGUI_VIEWPROVIDERCROSSHATCH_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <App/PropertyStandard.h>
#include <Gui/ViewProviderDocumentObject.h>

namespace App {
    class DocumentObject;
}

namespace TechDraw{
    class DrawGeomHatch;
}

namespace TechDrawGui {


class TechDrawGuiExport ViewProviderGeomHatch : public Gui::ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderGeomHatch);

public:
    /// constructor
    ViewProviderGeomHatch();
    /// destructor
    ~ViewProviderGeomHatch() override;

    App::PropertyFloat       WeightPattern;
    App::PropertyColor       ColorPattern;

    void updateData(const App::Property*) override;
    void onChanged(const App::Property *prop) override;
    bool setEdit(int ModNum) override;
    bool doubleClicked(void) override;
    bool useNewSelectionModel(void) const override {return false;}
    void updateGraphic(void);
    void getParameters(void);
    bool canDelete(App::DocumentObject* obj) const override;

    TechDraw::DrawGeomHatch* getViewObject() const;

    Gui::MDIView *getMDIView() const override;
};

} // namespace TechDrawGui

#endif // DRAWINGGUI_VIEWPROVIDERHATCH_H
