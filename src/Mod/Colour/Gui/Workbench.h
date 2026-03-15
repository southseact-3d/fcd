// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_GUI_WORKBENCH_H
#define COLOUR_GUI_WORKBENCH_H

#include <Gui/Workbench.h>

namespace ColourGui
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

}  // namespace ColourGui

#endif  // COLOUR_GUI_WORKBENCH_H
