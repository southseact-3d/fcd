#include "ViewProviderPointParametric.h"


using namespace PartGui;
using namespace std;

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderPointParametric, PartGui::ViewProviderPrimitive)


ViewProviderPointParametric::ViewProviderPointParametric()
{
    sPixmap = "Part_Point_Parametric";
}

ViewProviderPointParametric::~ViewProviderPointParametric() = default;


// **********************************************************************************

std::vector<std::string> ViewProviderPointParametric::getDisplayModes() const
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
