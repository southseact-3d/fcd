#include <Base/FileInfo.h>
#include <Base/Stream.h>

#include "DrawViewSymbol.h"
#include "DrawView.h"
// inclusion of the generated files
#include <Mod/TechDraw/App/DrawViewPy.h>
#include <Mod/TechDraw/App/DrawViewSymbolPy.h>
#include <Mod/TechDraw/App/DrawViewSymbolPy.cpp>


using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string DrawViewSymbolPy::representation() const
{
    return std::string("<DrawViewSymbol object>");
}

PyObject* DrawViewSymbolPy::dumpSymbol(PyObject *args)
{
    const char* fileSpec;
    if (!PyArg_ParseTuple(args, "s", &fileSpec)) {
        return nullptr;
    }

    std::string symbolRepr = getDrawViewSymbolPtr()->Symbol.getValue();
    Base::FileInfo fi(fileSpec);
    Base::ofstream outfile;
    outfile.open(fi);
    outfile.write (symbolRepr.c_str(), symbolRepr.size());
    outfile.close();
    if (outfile.good()) {
        outfile.close();
    } else {
        std::string error = std::string("Cannot write");
        error += fileSpec;
        PyErr_SetString(PyExc_RuntimeError, error.c_str());
        return nullptr;
    }

    Py_Return;
}

PyObject *DrawViewSymbolPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DrawViewSymbolPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
