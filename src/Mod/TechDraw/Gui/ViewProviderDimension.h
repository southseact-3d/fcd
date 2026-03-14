#ifndef DRAWINGGUI_VIEWPROVIDERDIMENSION_H
#define DRAWINGGUI_VIEWPROVIDERDIMENSION_H

#include <App/PropertyUnits.h>
#include <Mod/TechDraw/TechDrawGlobal.h>
#include <Mod/TechDraw/App/DrawViewDimension.h>

#include "ViewProviderDrawingView.h"


namespace TechDrawGui {

class TechDrawGuiExport ViewProviderDimension : public ViewProviderDrawingView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderDimension);

public:
    /// constructor
    ViewProviderDimension();
    /// destructor
    ~ViewProviderDimension() override = default;

    App::PropertyFont   Font;
    App::PropertyLength Fontsize;
    App::PropertyLength Arrowsize;
    App::PropertyEnumeration  ArrowStyle;
    App::PropertyLength LineWidth;
    App::PropertyColor  Color;

    static const int STD_STYLE_ISO_ORIENTED     = 0;
    static const int STD_STYLE_ISO_REFERENCING  = 1;
    static const int STD_STYLE_ASME_INLINED     = 2;
    static const int STD_STYLE_ASME_REFERENCING = 3;
    App::PropertyEnumeration StandardAndStyle;

    static const int REND_EXTENT_NONE     = 0;
    static const int REND_EXTENT_MINIMAL  = 1;
    static const int REND_EXTENT_CONFINED = 2;
    static const int REND_EXTENT_REDUCED  = 3;
    static const int REND_EXTENT_NORMAL   = 4;
    static const int REND_EXTENT_EXPANDED = 5;
    App::PropertyEnumeration RenderingExtent;

    App::PropertyBool        FlipArrowheads;

    App::PropertyFloat GapFactorISO;
    App::PropertyFloat GapFactorASME;
    App::PropertyFloat LineSpacingFactorISO;

    void attach(App::DocumentObject *pcFeat) override;
    bool useNewSelectionModel() const override {return false;}
    void updateData(const App::Property* prop) override;
    void onChanged(const App::Property* p) override;
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;
    bool setEdit(int ModNum) override;
    bool doubleClicked() override;
    bool onDelete(const std::vector<std::string> & parms) override;
    void finishRestoring() override;


    TechDraw::DrawViewDimension* getViewObject() const override;

    Base::Color prefColor() const;
    std::string prefFont() const;
    double prefFontSize() const;
    double prefArrowSize() const;
    double prefWeight() const;
    int prefStandardAndStyle() const;
    bool canDelete(App::DocumentObject* obj) const override;
    void setPixmapForType();

    std::vector<App::DocumentObject*> claimChildren() const override;

    void fixTextSize();
    void fixArrowSize();

protected:
    void handleChangedPropertyType(Base::XMLReader &reader, const char *TypeName, App::Property * prop) override;

private:
    static const char *StandardAndStyleEnums[];  // NOLINT
    static const char *RenderingExtentEnums[];   // NOLINT

};

} // namespace TechDrawGui


#endif // DRAWINGGUI_VIEWPROVIDERDIMENSION_H
