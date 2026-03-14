#ifndef MESHPARTGUI_WORKBENCH_H
#define MESHPARTGUI_WORKBENCH_H

#include <Gui/Workbench.h>

namespace MeshPartGui
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

}  // namespace MeshPartGui


#endif  // MESHPARTGUI_WORKBENCH_H
