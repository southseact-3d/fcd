#ifndef DRAWINGGUI_VIEWPROVIDERSPREADSHEET_H
#define DRAWINGGUI_VIEWPROVIDERSPREADSHEET_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <Mod/TechDraw/App/DrawViewSpreadsheet.h>

#include "ViewProviderSymbol.h"


namespace TechDrawGui {


class TechDrawGuiExport ViewProviderSpreadsheet : public ViewProviderSymbol
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderSpreadsheet);

public:
    /// constructor
    ViewProviderSpreadsheet();
    /// destructor
    ~ViewProviderSpreadsheet() override;

    App::PropertyBool ClaimSheetAsChild;
    std::vector<App::DocumentObject*> claimChildren(void) const override;

    bool useNewSelectionModel() const override {return false;}

    TechDraw::DrawViewSpreadsheet* getViewObject() const override;
};

} // namespace TechDrawGui


#endif // DRAWINGGUI_VIEWPROVIDERSPREADSHEET_H
