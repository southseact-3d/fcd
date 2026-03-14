#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>

#include "Workbench.h"


using namespace ReverseEngineeringGui;

#if 0  // needed for Qt's lupdate utility
    qApp->translate("Workbench", "Reverse Engineering");
#endif

/// @namespace ReverseEngineeringGui @class Workbench
TYPESYSTEM_SOURCE(ReverseEngineeringGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench() = default;

Workbench::~Workbench() = default;

Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Windows");
    Gui::MenuItem* reen = new Gui::MenuItem;
    root->insertItem(item, reen);
    reen->setCommand("&Reverse Engineering");

    Gui::MenuItem* reconstruct = new Gui::MenuItem();
    reconstruct->setCommand("Surface Reconstruction");
    *reconstruct << "Reen_PoissonReconstruction"
                 << "Reen_ViewTriangulation";
    *reen << reconstruct;

    Gui::MenuItem* segm = new Gui::MenuItem();
    segm->setCommand("Segmentation");
    *segm << "Mesh_RemeshGmsh"
          << "Mesh_VertexCurvature"
          << "Mesh_CurvatureInfo"
          << "Separator"
          << "Reen_Segmentation"
          << "Reen_SegmentationManual"
          << "Reen_SegmentationFromComponents"
          << "Reen_MeshBoundary";
    *reen << segm;

    Gui::MenuItem* approx = new Gui::MenuItem();
    approx->setCommand("Approximation");
    *approx << "Reen_ApproxPlane"
            << "Reen_ApproxCylinder"
            << "Reen_ApproxSphere"
            << "Reen_ApproxPolynomial"
            << "Separator"
            << "Reen_ApproxSurface"
            << "Reen_ApproxCurve";
    *reen << approx;

    return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
    Gui::ToolBarItem* part = new Gui::ToolBarItem(root);
    part->setCommand("Reverse Engineering");
    *part << "Reen_ApproxSurface";
    return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
    // Part tools
    Gui::ToolBarItem* root = new Gui::ToolBarItem;
    return root;
}
