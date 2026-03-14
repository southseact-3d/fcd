#include "ViewProviderMainPart.h"
#include <Mod/PartDesign/App/FeatureMainPart.h>

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderMainPart, PartGui::ViewProviderPart)

ViewProviderMainPart::ViewProviderMainPart()
{}

ViewProviderMainPart::~ViewProviderMainPart()
{}

std::vector<App::DocumentObject*> ViewProviderMainPart::claimChildren(void) const
{
    std::vector<App::DocumentObject*> temp;
    temp.push_back(getObject<PartDesign::MainPart>()->Sketch.getValue());

    return temp;
}
