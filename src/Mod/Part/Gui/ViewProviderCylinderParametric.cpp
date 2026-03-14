#include "ViewProviderCylinderParametric.h"


using namespace PartGui;
using namespace std;

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderCylinderParametric, PartGui::ViewProviderPrimitive)


ViewProviderCylinderParametric::ViewProviderCylinderParametric()
{
    sPixmap = "Part_Cylinder_Parametric";
}

ViewProviderCylinderParametric::~ViewProviderCylinderParametric() = default;


// **********************************************************************************

std::vector<std::string> ViewProviderCylinderParametric::getDisplayModes() const
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
