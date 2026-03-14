#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>

#include "Workbench.h"


using namespace SurfaceGui;

/// @namespace SurfaceGui @class Workbench
TYPESYSTEM_SOURCE(SurfaceGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench() = default;

Workbench::~Workbench() = default;

Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Windows");

    Gui::MenuItem* surface = new Gui::MenuItem;
    root->insertItem(item, surface);
    surface->setCommand("Surface");
    *surface << "Surface_Filling"
             << "Surface_GeomFillSurface"
             << "Surface_Sections"
             << "Surface_ExtendFace"
             << "Surface_CurveOnMesh"
             << "Surface_BlendCurve";
    /*
     *surface << "Surface_Cut";
     */

    return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();

    Gui::ToolBarItem* surface = new Gui::ToolBarItem(root);
    surface->setCommand("Surface");
    *surface << "Surface_Filling"
             << "Surface_GeomFillSurface"
             << "Surface_Sections"
             << "Surface_ExtendFace"
             << "Surface_CurveOnMesh"
             << "Surface_BlendCurve";
    /*
     *surface << "Surface_Cut";
     */

    return root;
}
