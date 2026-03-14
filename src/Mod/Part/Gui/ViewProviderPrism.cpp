#include "ViewProviderPrism.h"


using namespace PartGui;
using namespace std;

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderPrism, PartGui::ViewProviderPrimitive)


ViewProviderPrism::ViewProviderPrism()
{
    sPixmap = "Part_Prism_Parametric";
}

ViewProviderPrism::~ViewProviderPrism() = default;

// **********************************************************************************

std::vector<std::string> ViewProviderPrism::getDisplayModes() const
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

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderWedge, PartGui::ViewProviderPrimitive)


ViewProviderWedge::ViewProviderWedge()
{
    sPixmap = "Part_Wedge_Parametric";
}

ViewProviderWedge::~ViewProviderWedge() = default;

// **********************************************************************************

std::vector<std::string> ViewProviderWedge::getDisplayModes() const
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
