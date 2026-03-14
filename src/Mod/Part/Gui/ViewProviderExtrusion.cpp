#include "ViewProviderExtrusion.h"
#include <Mod/Part/App/FeatureExtrusion.h>


using namespace PartGui;

PROPERTY_SOURCE(PartGui::ViewProviderExtrusion, PartGui::ViewProviderPart)

ViewProviderExtrusion::ViewProviderExtrusion()
{
    sPixmap = "Part_Extrude.svg";
}

ViewProviderExtrusion::~ViewProviderExtrusion() = default;

std::vector<App::DocumentObject*> ViewProviderExtrusion::claimChildren() const
{
    std::vector<App::DocumentObject*> temp;
    temp.push_back(getObject<Part::Extrusion>()->Base.getValue());

    return temp;
}
