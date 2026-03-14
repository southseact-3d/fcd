#include "DrawProjGroupItem.h"
// inclusion of the generated files (generated out of DrawProjGroupItemPy.xml)
#include <Mod/TechDraw/App/DrawProjGroupItemPy.h>
#include <Mod/TechDraw/App/DrawProjGroupItemPy.cpp>


using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string DrawProjGroupItemPy::representation() const
{
    return std::string("<DrawProjGroupItem object>");
}

PyObject* DrawProjGroupItemPy::autoPosition(PyObject *args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    DrawProjGroupItem* item = getDrawProjGroupItemPtr();
    item->autoPosition();

    Py_Return;
}

PyObject *DrawProjGroupItemPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DrawProjGroupItemPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
