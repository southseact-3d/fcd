#ifndef Surface_WORKBENCH_H
#define Surface_WORKBENCH_H

#include <Gui/Workbench.h>

namespace SurfaceGui
{

class Workbench: public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    Workbench();
    ~Workbench() override;

protected:
    Gui::MenuItem* setupMenuBar() const override;
    Gui::ToolBarItem* setupToolBars() const override;
};

}  // namespace SurfaceGui


#endif  // Surface_WORKBENCH_H
