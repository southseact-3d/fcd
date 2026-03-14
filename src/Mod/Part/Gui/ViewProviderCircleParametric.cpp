#include "ViewProviderCircleParametric.h"

using namespace PartGui;
using namespace std;


//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderCircleParametric, PartGui::ViewProviderPrimitive)


ViewProviderCircleParametric::ViewProviderCircleParametric()
{
    sPixmap = "Part_Circle_Parametric";
}

ViewProviderCircleParametric::~ViewProviderCircleParametric() = default;


// **********************************************************************************

std::vector<std::string> ViewProviderCircleParametric::getDisplayModes() const
{
    // get the modes of the father
    std::vector<std::string> StrList;

    // add your own modes
    StrList.emplace_back("Wireframe");
    StrList.emplace_back("Points");

    return StrList;
}
