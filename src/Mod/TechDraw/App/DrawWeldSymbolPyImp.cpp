#include "DrawWeldSymbol.h"
// inclusion of the generated files (generated out of DrawWeldSymbolPy.xml)
#include <Mod/TechDraw/App/DrawWeldSymbolPy.h>
#include <Mod/TechDraw/App/DrawWeldSymbolPy.cpp>


using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string DrawWeldSymbolPy::representation() const
{
    return std::string("<DrawWeldSymbol object>");
}

//PyObject* DrawWeldSymbolPy::getTiles(PyObject *args)
//{
//    const char* fileSpec;
//    PyObject* pTile
//    if (!PyArg_ParseTuple(args, "O", &pTile)) {
//       throw Py::TypeError("getTiles expected DrawTile");
//    }
//    auto dws = getDrawWeldSymbolPtr();
////    auto dt  = pTile->getDrawTilePtr();
////TODO: finish this!
//    Py_Return;
//}

PyObject *DrawWeldSymbolPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DrawWeldSymbolPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
