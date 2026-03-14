#include <Python.h>


#include "FemPostObjectPy.h"
#include "FemPostObjectPy.cpp"

#ifdef FC_USE_VTK_PYTHON
# include <vtkDataSet.h>
# include <vtkPythonUtil.h>
#endif  // BUILD_FEM_VTK

using namespace Fem;

// returns a string which represent the object e.g. when printed in python
std::string FemPostObjectPy::representation() const
{
    std::stringstream str;
    str << "<FemPostObject object at " << getFemPostObjectPtr() << ">";

    return str.str();
}

PyObject* FemPostObjectPy::writeVTK(PyObject* args)
{
    char* filename;
    if (!PyArg_ParseTuple(args, "et", "utf-8", &filename)) {
        return nullptr;
    }

    std::string utf8Name(filename);
    PyMem_Free(filename);
    getFemPostObjectPtr()->writeVTK(utf8Name.c_str());

    Py_Return;
}

PyObject* FemPostObjectPy::getDataSet(PyObject* args)
{
#ifdef FC_USE_VTK_PYTHON
    // we take no arguments
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    // return python object for the dataset
    auto dataset = getFemPostObjectPtr()->getDataSet();
    if (dataset) {
        PyObject* py_algorithm = vtkPythonUtil::GetObjectFromPointer(dataset);
        return py_algorithm;
    }

    Py_Return;
#else
    (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "VTK python wrapper not available");
    return nullptr;
#endif
}

PyObject* FemPostObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int FemPostObjectPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
