#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>

#include "Workbench.h"

using namespace MatGui;


#if 0  // needed for Qt's lupdate utility
    qApp->translate("Workbench", "&Materials");
    qApp->translate("Workbench", "Materials");
#endif

/// @namespace MatGui @class Workbench
TYPESYSTEM_SOURCE(MatGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench() = default;

Workbench::~Workbench() = default;

Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Windows");

    Gui::MenuItem* material = new Gui::MenuItem;
    root->insertItem(item, material);
    material->setCommand("&Material");
    *material << "Material_Edit";

    return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();

    Gui::ToolBarItem* material = new Gui::ToolBarItem(root);
    material->setCommand("Material");
    *material << "Material_Edit";

    return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
    // Part tools
    Gui::ToolBarItem* root = new Gui::ToolBarItem;
    return root;
}
