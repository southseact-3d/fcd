#ifndef PARTDESIGN_WORKBENCH_H
#define PARTDESIGN_WORKBENCH_H

#include <Gui/Workbench.h>
#include <Mod/PartDesign/PartDesignGlobal.h>

namespace Gui
{

class MenuItem;

}

namespace PartDesignGui
{

/**
 * @author Werner Mayer
 */
class PartDesignGuiExport Workbench: public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    Workbench();
    ~Workbench() override;

    /** Run some actions when the workbench gets activated. */
    void activated() override;
    /** Run some actions when the workbench gets deactivated. */
    void deactivated() override;

    /// Add custom entries to the context menu
    void setupContextMenu(const char* recipient, Gui::MenuItem*) const override;

protected:
    Gui::MenuItem* setupMenuBar() const override;
    Gui::ToolBarItem* setupToolBars() const override;
};

}  // namespace PartDesignGui


#endif  // PARTDESIGN_WORKBENCH_H
