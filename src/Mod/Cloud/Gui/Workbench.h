#ifndef Cloud_WORKBENCH_H
#define Cloud_WORKBENCH_H

#include <Gui/Workbench.h>

namespace CloudGui
{

class Workbench: public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER();

public:
    Workbench();
    virtual ~Workbench();

protected:
    //    Gui::MenuItem* setupMenuBar() const;
    //    Gui::ToolBarItem* setupToolBars() const;
};

}  // namespace CloudGui


#endif  // Cloud_WORKBENCH_H
