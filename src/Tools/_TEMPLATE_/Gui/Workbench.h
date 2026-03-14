#ifndef _TEMPLATE__WORKBENCH_H
#define _TEMPLATE__WORKBENCH_H

#include <Gui/Workbench.h>

namespace _TEMPLATE_Gui
{

class Workbench: public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER();

public:
    Workbench();
    virtual ~Workbench();

protected:
    Gui::MenuItem* setupMenuBar() const;
    Gui::ToolBarItem* setupToolBars() const;
};

}  // namespace _TEMPLATE_Gui


#endif  // _TEMPLATE__WORKBENCH_H
