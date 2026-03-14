#include "ViewProviderSphereParametric.h"

using namespace PartGui;


//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(PartGui::ViewProviderSphereParametric, PartGui::ViewProviderPrimitive)

ViewProviderSphereParametric::ViewProviderSphereParametric()
{
    sPixmap = "Part_Sphere_Parametric";
}

ViewProviderSphereParametric::~ViewProviderSphereParametric() = default;

std::vector<std::string> ViewProviderSphereParametric::getDisplayModes() const
{
    std::vector<std::string> StrList;

    // add your own modes
    StrList.emplace_back("Flat Lines");
    StrList.emplace_back("Shaded");
    StrList.emplace_back("Wireframe");
    StrList.emplace_back("Points");

    return StrList;
}

// ----------------------------------------------------------------------------

PROPERTY_SOURCE(PartGui::ViewProviderEllipsoid, PartGui::ViewProviderPrimitive)

ViewProviderEllipsoid::ViewProviderEllipsoid()
{
    sPixmap = "Part_Ellipsoid_Parametric";
}

ViewProviderEllipsoid::~ViewProviderEllipsoid() = default;

std::vector<std::string> ViewProviderEllipsoid::getDisplayModes() const
{
    std::vector<std::string> StrList;

    // add your own modes
    StrList.emplace_back("Flat Lines");
    StrList.emplace_back("Shaded");
    StrList.emplace_back("Wireframe");
    StrList.emplace_back("Points");

    return StrList;
}
