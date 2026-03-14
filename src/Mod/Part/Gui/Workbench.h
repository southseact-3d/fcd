#ifndef PART_WORKBENCH_H
#define PART_WORKBENCH_H

#include <Gui/Workbench.h>
#include <Mod/Part/PartGlobal.h>

namespace PartGui
{

/**
 * @author Werner Mayer
 */
class PartGuiExport Workbench: public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    Workbench();
    ~Workbench() override;

private:
    bool hasSketcher = false;

protected:
    Gui::MenuItem* setupMenuBar() const override;
    Gui::ToolBarItem* setupToolBars() const override;
    Gui::ToolBarItem* setupCommandBars() const override;
};

}  // namespace PartGui


#endif  // PART_WORKBENCH_H
