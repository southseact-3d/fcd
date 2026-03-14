#include <QMenu>


#include "ViewProviderHelixParametric.h"

using namespace PartGui;


PROPERTY_SOURCE(PartGui::ViewProviderHelixParametric, PartGui::ViewProviderPrimitive)


ViewProviderHelixParametric::ViewProviderHelixParametric()
{
    sPixmap = "Part_Helix_Parametric";
    extension.initExtension(this);
}

ViewProviderHelixParametric::~ViewProviderHelixParametric() = default;

std::vector<std::string> ViewProviderHelixParametric::getDisplayModes() const
{
    // add your own modes
    std::vector<std::string> StrList;
    StrList.emplace_back("Wireframe");
    StrList.emplace_back("Points");

    return StrList;
}

void ViewProviderHelixParametric::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    ViewProviderPrimitive::setupContextMenu(menu, receiver, member);
}

// ------------------------------------------------------------------

PROPERTY_SOURCE(PartGui::ViewProviderSpiralParametric, PartGui::ViewProviderPrimitive)


ViewProviderSpiralParametric::ViewProviderSpiralParametric()
{
    sPixmap = "Part_Spiral_Parametric";
    extension.initExtension(this);
}

ViewProviderSpiralParametric::~ViewProviderSpiralParametric() = default;

std::vector<std::string> ViewProviderSpiralParametric::getDisplayModes() const
{
    // add your own modes
    std::vector<std::string> StrList;
    StrList.emplace_back("Wireframe");
    StrList.emplace_back("Points");

    return StrList;
}

void ViewProviderSpiralParametric::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    ViewProviderPrimitive::setupContextMenu(menu, receiver, member);
}
