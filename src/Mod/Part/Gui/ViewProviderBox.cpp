#include "ViewProviderBox.h"


using namespace PartGui;
using namespace std;

//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderBox, PartGui::ViewProviderPrimitive)


ViewProviderBox::ViewProviderBox()
{
    sPixmap = "Part_Box_Parametric";
}

ViewProviderBox::~ViewProviderBox() = default;


// **********************************************************************************

std::vector<std::string> ViewProviderBox::getDisplayModes() const
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
