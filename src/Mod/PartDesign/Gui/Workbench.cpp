// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Control.h>
#include <Gui/MDIView.h>
#include <Gui/MainWindow.h>
#include <Gui/ToolBarManager.h>
#include <Mod/Sketcher/Gui/Workbench.h>
#include <Mod/PartDesign/App/Body.h>
#include <Mod/PartDesign/App/FeatureMultiTransform.h>

#include <QToolBar>

#include "PartDesignPartToolsWidget.h"
#include "Utils.h"
#include "Workbench.h"
#include "WorkflowManager.h"

using namespace PartDesignGui;
namespace sp = std::placeholders;

#if 0  // needed for Qt's lupdate utility
    qApp->translate("Workbench", "&Sketch");
    //
    qApp->translate("Workbench", "&Part Design");
    qApp->translate("Workbench", "Datums");
    qApp->translate("Workbench", "Extrude");
    qApp->translate("Workbench", "Additive Features");
    qApp->translate("Workbench", "Subtractive Features");
    qApp->translate("Workbench", "Dress-Up Features");
    qApp->translate("Workbench", "Transformation Features");
    qApp->translate("Workbench", "Sprocket…");
    qApp->translate("Workbench", "Chain…");
    qApp->translate("Workbench", "Involute Gear");

    qApp->translate("Workbench", "Shaft Design Wizard");
    qApp->translate("Gui::TaskView::TaskWatcherCommands", "Face Tools");
    qApp->translate("Gui::TaskView::TaskWatcherCommands", "Edge Tools");
    qApp->translate("Gui::TaskView::TaskWatcherCommands", "Boolean Tools");
    qApp->translate("Gui::TaskView::TaskWatcherCommands", "Helper Tools");
    qApp->translate("Gui::TaskView::TaskWatcherCommands", "Modeling Tools");
    qApp->translate("Gui::TaskView::TaskWatcherCommands", "Create Geometry");
    //
    qApp->translate("Workbench", "Measure");
    qApp->translate("Workbench", "Refresh");
    qApp->translate("Workbench", "Toggle 3D");
    qApp->translate("Workbench", "Part Design Helper");
    qApp->translate("Workbench", "Part Design Modeling");
    qApp->translate("Workbench", "Part Primitives");
    qApp->translate("Workbench", "Part Boolean");
    qApp->translate("Workbench", "Part Join");
    qApp->translate("Workbench", "Part Split");
    qApp->translate("Workbench", "Part Compound");
    qApp->translate("Workbench", "Part Copy");
    qApp->translate("Workbench", "Part Design Part Tools");
    qApp->translate("Workbench", "Migration");
#endif

/// @namespace PartDesignGui @class Workbench
TYPESYSTEM_SOURCE(PartDesignGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench() = default;

Workbench::~Workbench()
{
    WorkflowManager::destruct();
}

void Workbench::setupContextMenu(const char* recipient, Gui::MenuItem* item) const
{
    auto selection = Gui::Selection().getSelection();
    // Add move Tip Command
    if (!selection.empty()) {
        App::DocumentObject* feature = selection.front().pObject;
        PartDesign::Body* body = nullptr;

        body = PartDesignGui::getBodyFor(feature, false, false, true);
        // lote of assertion so feature should be marked as a tip
        if (selection.size() == 1 && feature && body
            && (feature->isDerivedFrom<PartDesign::Feature>()
                || (feature->isDerivedFrom<Part::Feature>()
                    && body->BaseFeature.getValue() == feature))) {
            *item << "PartDesign_MoveTip";
        }

        if (strcmp(recipient, "Tree") == 0) {
            Gui::MDIView* activeView = Gui::Application::Instance->activeView();

            if (activeView) {
                if (feature && feature->isDerivedFrom<PartDesign::Body>()) {
                    *item << "Std_ToggleFreeze";
                }

                if (activeView->getAppDocument()->countObjectsOfType<PartDesign::Body>() > 0) {
                    bool addMoveFeature = true;
                    bool addMoveFeatureInTree = (body != nullptr);
                    for (auto sel : selection) {
                        // if at least one selected feature cannot be moved to a body
                        // disable the entry
                        if (addMoveFeature && !PartDesign::Body::isAllowed(sel.pObject)) {
                            addMoveFeature = false;
                        }
                        // if all at least one selected feature doesn't belong to the same body
                        // disable the menu entry
                        if (addMoveFeatureInTree && !body->hasObject(sel.pObject)) {
                            addMoveFeatureInTree = false;
                        }

                        if (!addMoveFeatureInTree && !addMoveFeature) {
                            break;
                        }
                    }
                    if (addMoveFeature) {
                        *item << "PartDesign_MoveFeature";
                    }
                    if (addMoveFeatureInTree) {
                        *item << "PartDesign_MoveFeatureInTree";
                    }
                }
            }
            if (Gui::Selection().countObjectsOfType<PartDesign::Transformed>()
                    - Gui::Selection().countObjectsOfType<PartDesign::MultiTransform>()
                == 1) {
                *item << "PartDesign_MultiTransform";
            }
        }
    }

    if (item->hasItems()) {
        *item << "Separator";
    }
    Gui::StdWorkbench::setupContextMenu(recipient, item);
}

void Workbench::activated()
{
    Gui::Workbench::activated();

    WorkflowManager::init();

    // Force the Part Design Part Tools toolbar visible — it may have been
    // hidden by stale user preferences (the old Mesh/Design toggle system).
    auto* mainWin = Gui::getMainWindow();
    if (mainWin) {
        auto* toolbar = mainWin->findChild<QToolBar*>(
            QStringLiteral("Part Design Part Tools"));
        if (toolbar) {
            toolbar->clear();
            toolbar->setVisible(true);
            auto* widget = new PartDesignPartToolsWidget(toolbar);
            toolbar->addWidget(widget);
        }
    }
}

void Workbench::deactivated()
{
    // reset the active Body
    Gui::Command::doCommand(Gui::Command::Doc, "import PartDesignGui");

    Gui::Workbench::deactivated();
}

Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Windows");

    // add another top level menu left besides the Part Design menu for the Sketcher commands
    Gui::MenuItem* sketch = new Gui::MenuItem;
    root->insertItem(item, sketch);
    sketch->setCommand("&Sketch");

    *sketch << "PartDesign_NewSketch"
            << "Sketcher_EditSketch"
            << "Sketcher_MapSketch"
            << "Sketcher_ReorientSketch"
            << "Sketcher_ValidateSketch"
            << "Sketcher_MergeSketches"
            << "Sketcher_MirrorSketch";

    Gui::MenuItem* part = new Gui::MenuItem;
    root->insertItem(item, part);
    part->setCommand("&Part Design");

    Gui::MenuItem* partPrimitives = new Gui::MenuItem;
    partPrimitives->setCommand("Part Primitives");
    *partPrimitives << "PartDesign_PartBox"
                    << "PartDesign_PartCylinder"
                    << "PartDesign_PartSphere"
                    << "PartDesign_PartCone"
                    << "PartDesign_PartTorus"
                    << "PartDesign_PartTube"
                    << "PartDesign_PartPrimitives";

    Gui::MenuItem* partBoolean = new Gui::MenuItem;
    partBoolean->setCommand("Part Boolean");
    *partBoolean << "PartDesign_PartBoolean"
                 << "PartDesign_PartCut"
                 << "PartDesign_PartFuse"
                 << "PartDesign_PartCommon"
                 << "PartDesign_PartSection";

    Gui::MenuItem* partJoin = new Gui::MenuItem;
    partJoin->setCommand("Part Join");
    *partJoin << "PartDesign_PartJoinConnect"
              << "PartDesign_PartJoinEmbed"
              << "PartDesign_PartJoinCutout";

    Gui::MenuItem* partSplit = new Gui::MenuItem;
    partSplit->setCommand("Part Split");
    *partSplit << "PartDesign_PartBooleanFragments"
               << "PartDesign_PartSliceApart"
               << "PartDesign_PartSlice"
               << "PartDesign_PartXor";

    Gui::MenuItem* partCompound = new Gui::MenuItem;
    partCompound->setCommand("Part Compound");
    *partCompound << "PartDesign_PartCompound"
                  << "PartDesign_PartExplodeCompound"
                  << "PartDesign_PartCompoundFilter"
                  << "PartDesign_PartToleranceSet";

    Gui::MenuItem* partCopy = new Gui::MenuItem;
    partCopy->setCommand("Part Copy");
    *partCopy << "PartDesign_PartSimpleCopy"
              << "PartDesign_PartTransformedCopy"
              << "PartDesign_PartElementCopy"
              << "PartDesign_PartRefineShape";

    Gui::MenuItem* migration = new Gui::MenuItem;
    migration->setCommand("Migration");
    *migration << "PartDesign_MigratePartMacros";

    // additives
    Gui::MenuItem* additives = new Gui::MenuItem;
    additives->setCommand("Additive Features");

    *additives << "PartDesign_Pad"
               << "PartDesign_Revolution"
               << "PartDesign_AdditiveLoft"
               << "PartDesign_AdditivePipe"
               << "PartDesign_AdditiveHelix";

    // subtractives
    Gui::MenuItem* subtractives = new Gui::MenuItem;
    subtractives->setCommand("Subtractive Features");

    *subtractives << "PartDesign_Pocket"
                  << "PartDesign_Hole"
                  << "PartDesign_Groove"
                  << "PartDesign_SubtractiveLoft"
                  << "PartDesign_SubtractivePipe"
                  << "PartDesign_SubtractiveHelix";

    // transformations
    Gui::MenuItem* transformations = new Gui::MenuItem;
    transformations->setCommand("Transformation Features");

    *transformations << "PartDesign_Mirrored"
                     << "PartDesign_LinearPattern"
                     << "PartDesign_PolarPattern"
                     << "PartDesign_MultiTransform";

    // dressups
    Gui::MenuItem* dressups = new Gui::MenuItem;
    dressups->setCommand("Dress-Up Features");

    *dressups << "PartDesign_Fillet"
              << "PartDesign_Chamfer"
              << "PartDesign_Draft"
              << "PartDesign_Thickness"
              << "PartDesign_BrickTexture";

    *part << "PartDesign_Body"
          << "Separator"
          << "PartDesign_ShapeBinder"
          << "PartDesign_SubShapeBinder"
          << "PartDesign_Clone"
          << "Separator"
          << "PartDesign_Extrude"
          << additives << "PartDesign_CompPrimitiveAdditive"
          << "Separator" << subtractives << "PartDesign_CompPrimitiveSubtractive"
          << "Separator" << dressups << "Separator" << transformations << "Separator"
          << "PartDesign_Boolean"
          << "Separator" << partPrimitives
          << "PartDesign_PartBuilder"
          << partBoolean
          << partJoin
          << partSplit
          << partCompound
          << partCopy
          << migration
          << "PartDesign_PartMakeSolid"
          << "PartDesign_PartReverseShape"
          << "PartDesign_PartMakeFace"
          << "PartDesign_PartLoft"
          << "PartDesign_PartSweep"
          << "PartDesign_PartThickness"
          << "PartDesign_PartProjectionOnSurface"
          << "PartDesign_PartSectionCut"
          << "PartDesign_PartEditAttachment"
          << "PartDesign_PartScale"
          << "PartDesign_PartOffset"
          << "PartDesign_PartOffset2D"
          << "PartDesign_PartShapeFromMesh"
          << "PartDesign_PartDefeaturing"
          << "PartDesign_PartCrossSections"
          << "PartDesign_PartRuledSurface"
          << "Separator"
          << "Materials_InspectAppearance"
          << "Materials_InspectMaterial"
          << "Separator"
          << "PartDesign_PartCheckGeometry"
          << "Separator"
          << "PartDesign_InvoluteGear"
          << "PartDesign_Sprocket"
          << "PartDesign_Chain";

    // For 0.13 a couple of python packages like numpy, matplotlib and others
    // are not deployed with the installer on Windows. Thus, the WizardShaft is
    // not deployed either hence the check for the existence of the command.
    if (Gui::Application::Instance->commandManager().getCommandByName("PartDesign_WizardShaft")) {
        *part << "Separator" << "PartDesign_WizardShaft";
    }

    Gui::MenuItem* view = root->findItem("&View");
    if (view) {
        Gui::MenuItem* appr = view->findItem("Std_RandomColor");
        appr = view->afterItem(appr);
        Gui::MenuItem* face = new Gui::MenuItem();
        face->setCommand("Part_ColorPerFace");
        view->insertItem(appr, face);
    }

    // Replace the "Duplicate selection" menu item with a replacement that is compatible with Body
    item = root->findItem("&Edit");
    Gui::MenuItem* dup = item->findItem("Std_DuplicateSelection");
    dup->setCommand("PartDesign_DuplicateSelection");

    return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();

    // Part Design Part Tools — replaced at activation with the custom
    // Create/Modify/Inspect grouped widget. We declare a minimal toolbar
    // here so that the toolbar exists and can be found by name; the actual
    // content is swapped in by Workbench::activated().
    Gui::ToolBarItem* part = new Gui::ToolBarItem(root);
    part->setCommand("Part Design Part Tools");
    part->setCustomIconSize(20);
    *part << "PartDesign_Pad";

    return root;
}
