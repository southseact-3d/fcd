#ifndef DRAWINGGUI_VIEWPROVIDERLEADER_H
#define DRAWINGGUI_VIEWPROVIDERLEADER_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <App/PropertyUnits.h>

#include <Mod/TechDraw/App/DrawLeaderLine.h>

#include "ViewProviderDrawingView.h"

namespace TechDraw {
class DrawRichAnno;
class DrawLeaderLine;
}

namespace TechDrawGui {

class TechDrawGuiExport ViewProviderLeader : public ViewProviderDrawingView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderLeader);

public:
    /// constructor
    ViewProviderLeader();
    /// destructor
    ~ViewProviderLeader() override = default;

    App::PropertyLength             LineWidth;
    App::PropertyEnumeration        LineStyle;
    App::PropertyColor              Color;
    App::PropertyBool               UseOldCoords;

    bool useNewSelectionModel() const override {return false;}
    void onChanged(const App::Property* p) override;
    bool setEdit(int ModNum) override;
    bool doubleClicked() override;
    bool onDelete(const std::vector<std::string> &parameters) override;
    bool canDelete(App::DocumentObject* obj) const override;

    static const char* LineStyleEnums[];

    std::vector<App::DocumentObject*> claimChildren() const override;

    TechDraw::DrawLeaderLine* getViewObject() const override;
    TechDraw::DrawLeaderLine* getFeature()  const;

protected:
    double getDefLineWeight();
    Base::Color getDefLineColor();
    void handleChangedPropertyType(Base::XMLReader &reader, const char *TypeName, App::Property * prop) override;

private:
    static App::PropertyIntegerConstraint::Constraints LineStyleRange;

};

} // namespace TechDrawGui


#endif // DRAWINGGUI_VIEWPROVIDERLEADER_H
