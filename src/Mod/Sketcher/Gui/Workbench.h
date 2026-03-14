#ifndef SKETCHER_WORKBENCH_H
#define SKETCHER_WORKBENCH_H

#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>
#include <Gui/Workbench.h>

#include <Mod/Sketcher/SketcherGlobal.h>


namespace SketcherGui
{

/**
 * @author Werner Mayer
 */
class SketcherGuiExport Workbench: public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    Workbench();
    ~Workbench() override;

    static void enterEditMode();
    static void leaveEditMode();

protected:
    Gui::MenuItem* setupMenuBar() const override;
    Gui::ToolBarItem* setupToolBars() const override;
    Gui::ToolBarItem* setupCommandBars() const override;
    void activated() override;
};

SketcherGuiExport void addSketcherWorkbenchSketchActions(Gui::MenuItem& sketch);
SketcherGuiExport void addSketcherWorkbenchSketchEditModeActions(Gui::MenuItem& sketch);
SketcherGuiExport void addSketcherWorkbenchGeometries(Gui::MenuItem& geom);
SketcherGuiExport void addSketcherWorkbenchConstraints(Gui::MenuItem& cons);
SketcherGuiExport void addSketcherWorkbenchTools(Gui::MenuItem& consaccel);
SketcherGuiExport void addSketcherWorkbenchBSplines(Gui::MenuItem& bspline);
SketcherGuiExport void addSketcherWorkbenchVisual(Gui::MenuItem& visual);

SketcherGuiExport void addSketcherWorkbenchSketchActions(Gui::ToolBarItem& sketch);
SketcherGuiExport void addSketcherWorkbenchSketchEditModeActions(Gui::ToolBarItem& sketch);
SketcherGuiExport void addSketcherWorkbenchGeometries(Gui::ToolBarItem& geom);
SketcherGuiExport void addSketcherWorkbenchConstraints(Gui::ToolBarItem& cons);
SketcherGuiExport void addSketcherWorkbenchTools(Gui::ToolBarItem& consaccel);
SketcherGuiExport void addSketcherWorkbenchBSplines(Gui::ToolBarItem& bspline);
SketcherGuiExport void addSketcherWorkbenchVisual(Gui::ToolBarItem& visual);

}  // namespace SketcherGui

#endif  // SKETCHER_WORKBENCH_H
