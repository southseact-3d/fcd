#include <App/DocumentObjectPy.h>
#include <App/FeaturePythonPyImp.h>

#include "FemSolverObject.h"


using namespace Fem;
using namespace App;

PROPERTY_SOURCE(Fem::FemSolverObject, App::DocumentObject)


FemSolverObject::FemSolverObject()
{
    ADD_PROPERTY_TYPE(
        Results,
        (nullptr),
        "Solver",
        App::PropertyType(App::Prop_ReadOnly | App::Prop_Output),
        "Solver results list"
    );
    ADD_PROPERTY_TYPE(
        WorkingDirectory,
        (""),
        "Solver",
        App::PropertyType(App::Prop_Transient | App::Prop_Hidden | App::Prop_Output),
        "Solver working directory"
    );
}

FemSolverObject::~FemSolverObject() = default;

short FemSolverObject::mustExecute() const
{
    return 0;
}

PyObject* FemSolverObject::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new DocumentObjectPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}

// Python feature ---------------------------------------------------------

namespace App
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Fem::FemSolverObjectPython, Fem::FemSolverObject)
template<>
const char* Fem::FemSolverObjectPython::getViewProviderName() const
{
    return "FemGui::ViewProviderSolverPython";
}

template<>
PyObject* Fem::FemSolverObjectPython::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new App::FeaturePythonPyT<App::DocumentObjectPy>(this), true);
    }
    return Py::new_reference_to(PythonObject);
}
/// @endcond
// explicit template instantiation
template class FemExport FeaturePythonT<Fem::FemSolverObject>;

}  // namespace App
