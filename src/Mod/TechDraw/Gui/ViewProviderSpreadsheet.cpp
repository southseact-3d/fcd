#include <App/DocumentObject.h>
#include "ViewProviderSpreadsheet.h"

using namespace TechDrawGui;

PROPERTY_SOURCE(TechDrawGui::ViewProviderSpreadsheet, TechDrawGui::ViewProviderSymbol)

//**************************************************************************
// Construction/Destruction

ViewProviderSpreadsheet::ViewProviderSpreadsheet()
{
    sPixmap = "TechDraw_TreeSpreadsheet";
    ADD_PROPERTY_TYPE(ClaimSheetAsChild,
                      (false),
                      "Display Options",
                      App::Prop_None,
                      "Claim (or not) the spreadsheet source as a child of the view.");
    LegacyScaling.setValue(true);
}

ViewProviderSpreadsheet::~ViewProviderSpreadsheet()
{
}

TechDraw::DrawViewSpreadsheet* ViewProviderSpreadsheet::getViewObject() const
{
    return dynamic_cast<TechDraw::DrawViewSpreadsheet*>(pcObject);
}

std::vector<App::DocumentObject*> ViewProviderSpreadsheet::claimChildren() const
{
    std::vector<App::DocumentObject*> temp;

    if (ClaimSheetAsChild.getValue()) {
        temp.push_back(getViewObject()->Source.getValue());
    }

    return temp;
}
