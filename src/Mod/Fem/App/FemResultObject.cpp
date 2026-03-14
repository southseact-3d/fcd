#include <App/DocumentObjectPy.h>
#include <App/FeaturePythonPyImp.h>

#include "FemResultObject.h"


using namespace Fem;
using namespace App;

PROPERTY_SOURCE(Fem::FemResultObject, App::DocumentObject)


FemResultObject::FemResultObject()
{
    ADD_PROPERTY_TYPE(Mesh, (nullptr), "General", Prop_None, "Link to the corresponding mesh");
    ADD_PROPERTY_TYPE(NodeNumbers, (0), "NodeData", Prop_None, "Numbers of the result nodes");
    ADD_PROPERTY_TYPE(Stats, (0), "Data", Prop_None, "Statistics of the results");
    ADD_PROPERTY_TYPE(Time, (0), "Data", Prop_None, "Time of analysis increment");

    // make read-only for property editor
    NodeNumbers.setStatus(App::Property::ReadOnly, true);
    Stats.setStatus(App::Property::ReadOnly, true);
    Time.setStatus(App::Property::ReadOnly, true);
}

FemResultObject::~FemResultObject() = default;

short FemResultObject::mustExecute() const
{
    return 0;
}

PyObject* FemResultObject::getPyObject()
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
PROPERTY_SOURCE_TEMPLATE(Fem::FemResultObjectPython, Fem::FemResultObject)
template<>
const char* Fem::FemResultObjectPython::getViewProviderName() const
{
    return "FemGui::ViewProviderResultPython";
}
/// @endcond

template<>
PyObject* Fem::FemResultObjectPython::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new App::FeaturePythonPyT<App::DocumentObjectPy>(this), true);
    }
    return Py::new_reference_to(PythonObject);
}

// explicit template instantiation
template class FemExport FeaturePythonT<Fem::FemResultObject>;

}  // namespace App
