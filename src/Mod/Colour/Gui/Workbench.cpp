// SPDX-License-Identifier: LGPL-2.1-or-later

#include "Workbench.h"

#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>

using namespace ColourGui;

TYPESYSTEM_SOURCE(ColourGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench() = default;

Workbench::~Workbench() = default;

Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Windows");
    Gui::MenuItem* colour = new Gui::MenuItem;
    root->insertItem(item, colour);
    colour->setCommand("Colour");
    *colour << "Colour_CreateColourMap"
            << "Separator"
            << "Colour_FaceFill"
            << "Colour_PaintMode"
            << "Colour_SketchOnFace"
            << "Colour_RefreshSketch"
            << "Colour_ImportTexture"
            << "Separator"
            << "Colour_LayerAdd"
            << "Colour_LayerDelete"
            << "Colour_LayerToggle"
            << "Colour_LayerPanel"
            << "Separator"
            << "Colour_SetMetallic"
            << "Colour_SetRoughness"
            << "Colour_SetAlpha"
            << "Colour_MaterialPanel"
            << "Separator"
            << "Colour_ExportTextured";
    return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
    Gui::ToolBarItem* colour = new Gui::ToolBarItem(root);
    colour->setCommand("Colour Paint");
    *colour << "Colour_CreateColourMap"
            << "Separator"
            << "Colour_FaceFill"
            << "Colour_PaintMode"
            << "Colour_SketchOnFace"
            << "Colour_RefreshSketch"
            << "Colour_ImportTexture"
            << "Separator"
            << "Colour_LayerAdd"
            << "Colour_LayerDelete"
            << "Colour_LayerPanel"
            << "Separator"
            << "Colour_SetMetallic"
            << "Colour_SetRoughness"
            << "Colour_SetAlpha"
            << "Separator"
            << "Colour_ExportTextured";
    return root;
}
