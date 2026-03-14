#include "ViewProviderLineParametric.h"


using namespace PartGui;
using namespace std;

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderLineParametric, PartGui::ViewProviderPrimitive)


ViewProviderLineParametric::ViewProviderLineParametric()
{
    sPixmap = "Part_Line_Parametric";
}

ViewProviderLineParametric::~ViewProviderLineParametric() = default;


// **********************************************************************************

std::vector<std::string> ViewProviderLineParametric::getDisplayModes() const
{
    // get the modes of the father
    std::vector<std::string> StrList;

    // add your own modes
    StrList.emplace_back("Wireframe");
    StrList.emplace_back("Points");

    return StrList;
}
