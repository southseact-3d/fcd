#ifndef GUI_SELECTIONFILTERPY_H
#define GUI_SELECTIONFILTERPY_H

#include <CXX/Extensions.hxx>

#include "SelectionFilter.h"


namespace Gui
{
/**
 * Python binding for SelectionFilter class.
 * \code
 * filter=Gui.Selection.Filter("SELECT Part::Feature SUBELEMENT Edge")
 * Gui.Selection.addSelectionGate(filter)
 * \endcode
 * @see SelectionFilter
 * @author Werner Mayer
 */
class SelectionFilterPy: public Py::PythonClass<SelectionFilterPy>
{
public:
    SelectionFilter filter;

public:
    static void init_type();  // announce properties and methods
    static SelectionFilterPy* cast(PyObject* py)
    {
        using SelectionFilterClass = Py::PythonClassObject<SelectionFilterPy>;
        return SelectionFilterClass(py).getCxxObject();
    }

    SelectionFilterPy(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kdws);

    ~SelectionFilterPy() override;

    Py::Object repr() override;
    Py::Object match();
    Py::Object result();
    Py::Object test(const Py::Tuple&);
    Py::Object setFilter(const Py::Tuple&);
    Py::Object getFilter();
};

}  // namespace Gui

#endif  // GUI_SELECTIONFILTERPY_H
