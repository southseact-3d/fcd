#include <Gui/WorkbenchManipulator.h>

namespace StartGui
{

class Manipulator: public Gui::WorkbenchManipulator
{
protected:
    /*!
     * This manipulator injects the command to load the start page into the Help menu.
     */
    void modifyMenuBar(Gui::MenuItem* menuBar) override;
};

}  // namespace StartGui

#endif  // FREECAD_MANIPULATOR_H
