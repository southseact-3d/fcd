#include "ViewProviderImport.h"


using namespace PartGui;

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderImport, PartGui::ViewProviderPart)

ViewProviderImport::ViewProviderImport()
{
    sPixmap = "Part_FeatureImport";
}

ViewProviderImport::~ViewProviderImport() = default;

// **********************************************************************************

bool ViewProviderImport::setEdit(int ModNum)
{
    return ViewProviderPart::setEdit(ModNum);
}

void ViewProviderImport::unsetEdit(int ModNum)
{
    ViewProviderPart::unsetEdit(ModNum);
}
