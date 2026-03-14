#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>

#include "Workbench.h"


using namespace PointsGui;

#if 0  // needed for Qt's lupdate utility
    qApp->translate("Workbench", "Points Tools");
    qApp->translate("Workbench", "&Points");
#endif

/// @namespace PointsGui @class Workbench
TYPESYSTEM_SOURCE(PointsGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench() = default;

Workbench::~Workbench() = default;

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
    Gui::ToolBarItem* pnt = new Gui::ToolBarItem(root);
    pnt->setCommand("Points Tools");
    *pnt << "Points_Import"
         << "Points_Export"
         << "Separator"
         << "Points_Convert"
         << "Points_Structure"
         << "Points_Merge"
         << "Points_PolyCut";
    return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
    // point tools
    Gui::ToolBarItem* root = new Gui::ToolBarItem;
    Gui::ToolBarItem* pnt = new Gui::ToolBarItem(root);
    pnt->setCommand("Points Tools");
    *pnt << "Points_Import"
         << "Points_Export"
         << "Points_Convert"
         << "Points_Structure"
         << "Points_Merge";
    return root;
}

Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Windows");
    Gui::MenuItem* pnts = new Gui::MenuItem;
    root->insertItem(item, pnts);

    pnts->setCommand("&Points");
    *pnts << "Points_Convert"
          << "Points_Structure"
          << "Separator"
          << "Points_Import"
          << "Points_Export"
          << "Separator"
          << "Points_PolyCut"
          << "Points_Merge";
    return root;
}
