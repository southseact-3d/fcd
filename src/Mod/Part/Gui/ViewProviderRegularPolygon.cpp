#include "ViewProviderRegularPolygon.h"


using namespace PartGui;
using namespace std;

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderRegularPolygon, PartGui::ViewProviderPrimitive)


ViewProviderRegularPolygon::ViewProviderRegularPolygon()
{
    sPixmap = "Part_Polygon_Parametric";
}

ViewProviderRegularPolygon::~ViewProviderRegularPolygon() = default;


// **********************************************************************************

std::vector<std::string> ViewProviderRegularPolygon::getDisplayModes() const
{
    // get the modes of the father
    std::vector<std::string> StrList;

    // add your own modes
    StrList.emplace_back("Wireframe");
    StrList.emplace_back("Points");

    return StrList;
}
