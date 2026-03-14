#ifndef DRAWINGGUI_VIEWPROVIDERVIEWPART_H
#define DRAWINGGUI_VIEWPROVIDERVIEWPART_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <App/PropertyUnits.h>

#include <Mod/TechDraw/App/DrawViewPart.h>

#include "ViewProviderDrawingView.h"


namespace TechDrawGui {

class TechDrawGuiExport ViewProviderViewPart : public ViewProviderDrawingView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderViewPart);

public:
    /// constructor
    ViewProviderViewPart();
    /// destructor
    ~ViewProviderViewPart() override;

    App::PropertyLength LineWidth;
    App::PropertyLength HiddenWidth;
    App::PropertyLength IsoWidth;
    App::PropertyLength ExtraWidth;
    App::PropertyBool   ArcCenterMarks;
    App::PropertyFloat  CenterScale;
    App::PropertyBool   HorizCenterLine;
    App::PropertyBool   VertCenterLine;
    App::PropertyBool   ShowSectionLine;
    App::PropertyBool   IncludeCutLine;
    App::PropertyEnumeration   SectionLineStyle;
    App::PropertyColor  SectionLineColor;
    App::PropertyBool   SectionLineMarks;
    App::PropertyEnumeration   HighlightLineStyle;
    App::PropertyColor  HighlightLineColor;
    App::PropertyFloat  HighlightAdjust;
    App::PropertyEnumeration BreakLineType;
    App::PropertyEnumeration BreakLineStyle;
    App::PropertyBool   ShowAllEdges;
    App::PropertyColor   FaceColor;
    App::PropertyPercent FaceTransparency;

    void attach(App::DocumentObject *) override;
    bool useNewSelectionModel() const override {return false;}
    bool onDelete(const std::vector<std::string> &) override;
    bool canDelete(App::DocumentObject* obj) const override;
    bool setEdit(int ModNum) override;
    bool setDetailEdit(int ModNum, TechDraw::DrawViewDetail* dvd);
    bool doubleClicked() override;
    void onChanged(const App::Property *prop) override;
    void handleChangedPropertyType(Base::XMLReader &reader, const char *TypeName, App::Property * prop) override;
    Base::Color prefSectionColor();
    Base::Color prefHighlightColor();
    int prefHighlightStyle();

    std::vector<App::DocumentObject*> claimChildren() const override;
    void fixSceneDependencies() override;

    TechDraw::DrawViewPart* getViewObject() const override;
    TechDraw::DrawViewPart* getViewPart() const;
};

} // namespace TechDrawGui


#endif // DRAWINGGUI_VIEWPROVIDERVIEWPART_H
