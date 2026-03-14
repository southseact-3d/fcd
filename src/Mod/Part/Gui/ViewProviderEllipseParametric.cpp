#include "ViewProviderEllipseParametric.h"


using namespace PartGui;
using namespace std;

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderEllipseParametric, PartGui::ViewProviderPrimitive)


ViewProviderEllipseParametric::ViewProviderEllipseParametric()
{
    sPixmap = "Part_Ellipse_Parametric";
}

ViewProviderEllipseParametric::~ViewProviderEllipseParametric() = default;


// **********************************************************************************

std::vector<std::string> ViewProviderEllipseParametric::getDisplayModes() const
{
    // get the modes of the father
    std::vector<std::string> StrList;

    // add your own modes
    StrList.emplace_back("Wireframe");
    StrList.emplace_back("Points");

    return StrList;
}
