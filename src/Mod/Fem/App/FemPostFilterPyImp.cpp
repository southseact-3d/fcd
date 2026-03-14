#include <Python.h>

#include <Base/FileInfo.h>
#include <Base/UnitPy.h>

// clang-format off
#include "FemPostGroupExtension.h"
#include "FemPostFilter.h"
#include "FemPostFilterPy.h"
#include "FemPostFilterPy.cpp"
// clang-format on

#ifdef FC_USE_VTK_PYTHON
# include <vtkUnstructuredGrid.h>
# include <vtkPythonUtil.h>
# include <vtkPolyData.h>
#endif  // BUILD_FEM_VTK

using namespace Fem;

// returns a string which represents the object e.g. when printed in python
std::string FemPostFilterPy::representation() const
{
    std::stringstream str;
    str << "<FemPostFilter object at " << getFemPostFilterPtr() << ">";

    return str.str();
}


PyObject* FemPostFilterPy::addFilterPipeline(PyObject* args)
{
#ifdef FC_USE_VTK_PYTHON
    const char* name;
    PyObject* source = nullptr;
    PyObject* target = nullptr;
    PyTypeObject* typeAlgo = vtkPythonUtil::FindClassTypeObject("vtkAlgorithm");
    if (!PyArg_ParseTuple(args, "sO!O!", &name, typeAlgo, &source, typeAlgo, &target)) {
        return nullptr;
    }

    // extract the algorithms
    vtkObjectBase* obj = vtkPythonUtil::GetPointerFromObject(source, "vtkAlgorithm");
    if (!obj) {
        // error marker is set by PythonUtil
        return nullptr;
    }
    auto source_algo = static_cast<vtkAlgorithm*>(obj);

    obj = vtkPythonUtil::GetPointerFromObject(target, "vtkAlgorithm");
    if (!obj) {
        // error marker is set by PythonUtil
        return nullptr;
    }
    auto target_algo = static_cast<vtkAlgorithm*>(obj);

    // add the pipeline
    FemPostFilter::FilterPipeline pipe;
    pipe.source = source_algo;
    pipe.target = target_algo;
    getFemPostFilterPtr()->addFilterPipeline(pipe, name);

    Py_Return;
#else
    (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "VTK python wrapper not available");
    return nullptr;
#endif
}

PyObject* FemPostFilterPy::setActiveFilterPipeline(PyObject* args)
{
    const char* name;
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return nullptr;
    }

    getFemPostFilterPtr()->setActiveFilterPipeline(std::string(name));

    Py_Return;
}

PyObject* FemPostFilterPy::getParentPostGroup(PyObject* args)
{
    // we take no arguments
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    auto group = Fem::FemPostGroupExtension::getGroupOfObject(getFemPostFilterPtr());
    if (group) {
        return group->getPyObject();
    }

    Py_Return;
}

PyObject* FemPostFilterPy::getInputData(PyObject* args)
{
#ifdef FC_USE_VTK_PYTHON
    // we take no arguments
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    // make a copy of the dataset
    auto dataset = getFemPostFilterPtr()->getInputData();
    vtkDataSet* copy;
    switch (dataset->GetDataObjectType()) {
        case VTK_UNSTRUCTURED_GRID:
            copy = vtkUnstructuredGrid::New();
            break;
        case VTK_POLY_DATA:
            copy = vtkPolyData::New();
            break;
        default:
            PyErr_SetString(PyExc_TypeError, "cannot return datatype object; not unstructured grid");
            return nullptr;
    }

    // return the python wrapper
    copy->DeepCopy(dataset);
    PyObject* py_dataset = vtkPythonUtil::GetObjectFromPointer(copy);

    return py_dataset;
#else
    (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "VTK python wrapper not available");
    return nullptr;
#endif
}

PyObject* FemPostFilterPy::getInputVectorFields(PyObject* args)
{
    // we take no arguments
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    std::vector<std::string> vector_fields = getFemPostFilterPtr()->getInputVectorFields();

    // convert to python list of strings
    Py::List list;
    for (std::string& field : vector_fields) {
        list.append(Py::String(field));
    }

    return Py::new_reference_to(list);
}


PyObject* FemPostFilterPy::getInputScalarFields(PyObject* args)
{
    // we take no arguments
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    std::vector<std::string> scalar_fields = getFemPostFilterPtr()->getInputScalarFields();

    // convert to python list of strings
    Py::List list;
    for (std::string& field : scalar_fields) {
        list.append(Py::String(field));
    }

    return Py::new_reference_to(list);
}

PyObject* FemPostFilterPy::getOutputAlgorithm(PyObject* args)
{
#ifdef FC_USE_VTK_PYTHON
    // we take no arguments
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    // return python object for the algorithm
    auto algorithm = getFemPostFilterPtr()->getFilterOutput();
    PyObject* py_algorithm = vtkPythonUtil::GetObjectFromPointer(algorithm);

    return py_algorithm;
#else
    (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "VTK python wrapper not available");
    return nullptr;
#endif
}

PyObject* FemPostFilterPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int FemPostFilterPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
