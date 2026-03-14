#include "ViewProviderScale.h"
#include <Mod/Part/App/FeatureScale.h>


using namespace PartGui;

PROPERTY_SOURCE(PartGui::ViewProviderScale, PartGui::ViewProviderPart)

ViewProviderScale::ViewProviderScale()
{
    sPixmap = "Part_Scale.svg";
}

ViewProviderScale::~ViewProviderScale() = default;

std::vector<App::DocumentObject*> ViewProviderScale::claimChildren() const
{
    std::vector<App::DocumentObject*> temp;
    temp.push_back(getObject<Part::Scale>()->Base.getValue());

    return temp;
}
