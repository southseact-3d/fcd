#ifndef GUI_WORKBENCHMANIPULATOR_PYTHON_H
#define GUI_WORKBENCHMANIPULATOR_PYTHON_H

#include <Gui/WorkbenchManipulator.h>
#include <CXX/Objects.hxx>

namespace Gui
{

/**
 * The WorkbenchManipulatorPython class accepts an instance of a Python class
 * that is supposed to implement any of the virtual functions.
 * @author Werner Mayer
 */
class GuiExport WorkbenchManipulatorPython: public WorkbenchManipulator
{
public:
    static void installManipulator(const Py::Object& obj);
    static void removeManipulator(const Py::Object& obj);
    explicit WorkbenchManipulatorPython(const Py::Object& obj);
    ~WorkbenchManipulatorPython() override;

protected:
    /*!
     * \brief modifyMenuBar
     * Method to manipulate the menu structure of a workbench.
     */
    void modifyMenuBar(MenuItem* menuBar) override;
    /*!
     * \brief modifyContextMenu
     * Method to manipulate the contextmenu structure of a workbench.
     */
    void modifyContextMenu(const char* recipient, MenuItem* menuBar) override;
    /*!
     * \brief modifyToolBars
     * Method to manipulate the toolbar structure of a workbench
     */
    void modifyToolBars(ToolBarItem* toolBar) override;
    /*!
     * \brief modifyDockWindows
     * Method to manipulate the dock window structure of a workbench
     */
    void modifyDockWindows(DockWindowItems* dockWindow) override;

public:
    WorkbenchManipulatorPython(const WorkbenchManipulatorPython&) = delete;
    WorkbenchManipulatorPython(WorkbenchManipulatorPython&&) = delete;
    WorkbenchManipulatorPython& operator=(const WorkbenchManipulatorPython&) = delete;
    WorkbenchManipulatorPython& operator=(WorkbenchManipulatorPython&&) = delete;

private:
    void tryModifyMenuBar(MenuItem* menuBar);
    void tryModifyMenuBar(const Py::Dict& dict, MenuItem* menuBar);
    void tryModifyContextMenu(const char* recipient, MenuItem* menuBar);
    void tryModifyContextMenu(const Py::Dict& dict, MenuItem* menuBar);
    void tryModifyToolBar(ToolBarItem* toolBar);
    void tryModifyToolBar(const Py::Dict& dict, ToolBarItem* toolBar);
    void tryModifyDockWindows(DockWindowItems* menuBar);
    void tryModifyDockWindows(const Py::Dict& dict, DockWindowItems* dockWindow);

private:
    Py::Object object;
};

}  // namespace Gui


#endif  // GUI_WORKBENCHMANIPULATOR_PYTHON_H
