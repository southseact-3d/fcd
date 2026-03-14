#include "ViewProviderTorusParametric.h"

using namespace PartGui;
using namespace std;


//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderTorusParametric, PartGui::ViewProviderPrimitive)


ViewProviderTorusParametric::ViewProviderTorusParametric()
{
    sPixmap = "Part_Torus_Parametric";
}

ViewProviderTorusParametric::~ViewProviderTorusParametric() = default;

// **********************************************************************************

std::vector<std::string> ViewProviderTorusParametric::getDisplayModes() const
{
    // get the modes of the father
    std::vector<std::string> StrList;

    // add your own modes
    StrList.emplace_back("Flat Lines");
    StrList.emplace_back("Shaded");
    StrList.emplace_back("Wireframe");
    StrList.emplace_back("Points");

    return StrList;
}
