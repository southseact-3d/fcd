#ifndef TECHDRAWGUI_VIEWPROVIDERRICHANNO_H
#define TECHDRAWGUI_VIEWPROVIDERRICHANNO_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <App/PropertyUnits.h>
#include <Mod/TechDraw/App/DrawRichAnno.h>

#include "ViewProviderDrawingView.h"


namespace TechDraw {
class DrawRichAnno;
}

namespace TechDrawGui {

class TechDrawGuiExport ViewProviderRichAnno : public ViewProviderDrawingView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderRichAnno);

public:
    /// constructor
    ViewProviderRichAnno();
    /// destructor
    ~ViewProviderRichAnno() override = default;

    App::PropertyLength      LineWidth;
    App::PropertyEnumeration LineStyle;
    App::PropertyColor       LineColor;

    bool useNewSelectionModel() const override {return false;}
    void updateData(const App::Property* prop) override;
    void onChanged(const App::Property* prop) override;
    bool doubleClicked() override;
    bool canDelete(App::DocumentObject* obj) const override;

    static const char* LineStyleEnums[];

    TechDraw::DrawRichAnno* getViewObject() const override;
    TechDraw::DrawRichAnno* getFeature()  const;

    /// Claim any views that have this as a parent
    std::vector<App::DocumentObject*> claimChildren() const override;


protected:
    Base::Color getDefLineColor();
    std::string getDefFont();
    double getDefFontSize();
    double getDefLineWeight();
    void handleChangedPropertyType(Base::XMLReader &reader,
                                   const char *TypeName,
                                   App::Property * prop) override;

private:
    static App::PropertyIntegerConstraint::Constraints LineStyleRange;

};

} // namespace TechDrawGui

#endif // TECHDRAWGUI_VIEWPROVIDERRICHANNO_H
