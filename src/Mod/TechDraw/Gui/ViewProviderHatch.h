#ifndef DRAWINGGUI_VIEWPROVIDERHATCH_H
#define DRAWINGGUI_VIEWPROVIDERHATCH_H

#include <Mod/TechDraw/TechDrawGlobal.h>
#include <App/PropertyGeo.h>

#include <Gui/ViewProviderDocumentObject.h>

namespace TechDraw{
    class DrawHatch;
}

namespace TechDrawGui {


class TechDrawGuiExport ViewProviderHatch : public Gui::ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderHatch);

public:
    /// constructor
    ViewProviderHatch();
    /// destructor
    ~ViewProviderHatch() override;

    App::PropertyColor           HatchColor;
    App::PropertyFloatConstraint HatchScale;
    App::PropertyFloat           HatchRotation;
    App::PropertyVector          HatchOffset;

    bool useNewSelectionModel() const override {return false;}
    void onChanged(const App::Property* prop) override;
    void updateData(const App::Property*) override;
    bool setEdit(int ModNum) override;
    bool doubleClicked() override;
    bool canDelete(App::DocumentObject* obj) const override;

    TechDraw::DrawHatch* getViewObject() const;

    Gui::MDIView *getMDIView() const override;

private:
    static App::PropertyFloatConstraint::Constraints scaleRange;

};

} // namespace TechDrawGui


#endif // DRAWINGGUI_VIEWPROVIDERHATCH_H
