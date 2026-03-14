#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>

#include "Sheet.h"


namespace Spreadsheet
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("Spreadsheet")
    {
        initialize("This module is the Spreadsheet module.");  // register with Python
    }

private:
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}
}  // namespace Spreadsheet

/* Python entry */
PyMOD_INIT_FUNC(Spreadsheet)
{
    Spreadsheet::PropertySpreadsheetQuantity::init();
    Spreadsheet::PropertyColumnWidths::init();
    Spreadsheet::PropertyRowHeights::init();
    Spreadsheet::PropertySheet::init();

    Spreadsheet::Sheet::init();
    Spreadsheet::SheetPython::init();

    PyObject* mod = Spreadsheet::initModule();
    Base::Console().log("Loading Spreadsheet module... done\n");
    PyMOD_Return(mod);
}
