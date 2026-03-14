#include "ViewProviderConeParametric.h"


using namespace PartGui;
using namespace std;

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderConeParametric, PartGui::ViewProviderPrimitive)


ViewProviderConeParametric::ViewProviderConeParametric()
{
    sPixmap = "Part_Cone_Parametric";
}

ViewProviderConeParametric::~ViewProviderConeParametric() = default;


// **********************************************************************************

std::vector<std::string> ViewProviderConeParametric::getDisplayModes() const
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
