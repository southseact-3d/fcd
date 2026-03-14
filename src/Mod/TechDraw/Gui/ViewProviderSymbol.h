#ifndef DRAWINGGUI_VIEWPROVIDERSYMBOL_H
#define DRAWINGGUI_VIEWPROVIDERSYMBOL_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <Mod/TechDraw/App/DrawViewSymbol.h>

#include "ViewProviderDrawingView.h"


namespace TechDrawGui {

//NOLINTBEGIN
class TechDrawGuiExport ViewProviderSymbol : public ViewProviderDrawingView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderSymbol);
//NOLINTEND

public:
    /// constructor
    ViewProviderSymbol();
    /// destructor
    ~ViewProviderSymbol() override  = default ;

    App::PropertyBool LegacyScaling;

    bool useNewSelectionModel() const override {return false;}
    void updateData(const App::Property* prop) override;
    void onChanged(const App::Property* prop) override;

    TechDraw::DrawViewSymbol* getViewObject() const override;
};

//NOLINTBEGIN
class TechDrawGuiExport ViewProviderDraft : public ViewProviderSymbol
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderDraft);
//NOLINTEND

public:
    /// constructor
    ViewProviderDraft();
    /// destructor
    ~ViewProviderDraft() override = default ;
};


//NOLINTBEGIN
class TechDrawGuiExport ViewProviderArch : public ViewProviderSymbol
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderArch);
//NOLINTEND

public:
    /// constructor
    ViewProviderArch();
    /// destructor
    ~ViewProviderArch() override  = default ;
};

} // namespace TechDrawGui


#endif // DRAWINGGUI_VIEWPROVIDERSYMBOL_H
