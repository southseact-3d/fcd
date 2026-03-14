#ifndef REENGUI_WORKBENCH_H
#define REENGUI_WORKBENCH_H

#include <Gui/Workbench.h>

namespace ReverseEngineeringGui
{

/**
 * @author Werner Mayer
 */
class Workbench: public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    Workbench();
    ~Workbench() override;

protected:
    Gui::MenuItem* setupMenuBar() const override;
    Gui::ToolBarItem* setupToolBars() const override;
    Gui::ToolBarItem* setupCommandBars() const override;
};

}  // namespace ReverseEngineeringGui


#endif  // REENGUI_WORKBENCH_H
