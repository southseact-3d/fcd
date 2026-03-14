#ifndef IMPORT_WORKBENCH_H
#define IMPORT_WORKBENCH_H

#include <Gui/Workbench.h>

namespace ImportGui
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
    Gui::ToolBarItem* setupToolBars() const override;
    Gui::ToolBarItem* setupCommandBars() const override;
};

}  // namespace ImportGui


#endif  // IMPORT_WORKBENCH_H
