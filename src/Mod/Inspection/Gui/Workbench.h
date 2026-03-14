#ifndef Inspection_WORKBENCH_H
#define Inspection_WORKBENCH_H

#include <Gui/Workbench.h>

namespace InspectionGui
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

}  // namespace InspectionGui


#endif  // Inspection_WORKBENCH_H
