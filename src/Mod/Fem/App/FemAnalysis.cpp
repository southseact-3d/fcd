#include <App/DocumentObjectPy.h>
#include <App/FeaturePythonPyImp.h>
#include <Base/Uuid.h>

#include "FemAnalysis.h"


using namespace Fem;
using namespace App;

PROPERTY_SOURCE(Fem::FemAnalysis, App::DocumentObjectGroup)


FemAnalysis::FemAnalysis()
{
    Base::Uuid id;
    ADD_PROPERTY_TYPE(Uid, (id), 0, App::Prop_None, "UUID of the Analysis");
}

FemAnalysis::~FemAnalysis() = default;

void FemAnalysis::handleChangedPropertyName(
    Base::XMLReader& reader,
    const char* TypeName,
    const char* PropName
)
{
    Base::Type type = Base::Type::fromName(TypeName);
    if (Group.getClassTypeId() == type && strcmp(PropName, "Member") == 0) {
        Group.Restore(reader);
    }
    else {
        App::DocumentObjectGroup::handleChangedPropertyName(reader, TypeName, PropName);
    }
}


// Dummy class 'DocumentObject' in Fem namespace
PROPERTY_SOURCE_ABSTRACT(Fem::DocumentObject, App::DocumentObject)

// Python feature ---------------------------------------------------------

namespace App
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Fem::FemAnalysisPython, Fem::FemAnalysis)
template<>
const char* Fem::FemAnalysisPython::getViewProviderName() const
{
    return "FemGui::ViewProviderFemAnalysisPython";
}

// template<> void Fem::FemAnalysisPython::Restore(Base::XMLReader& reader) {
//     FemAnalysis::Restore(reader);
// }
// template<> PyObject* Fem::FemAnalysisPython::getPyObject(void) {
//    if (PythonObject.is(Py::_None())) {
//        // ref counter is set to 1
//        PythonObject = Py::Object(new App::DocumentObjectPy(this),true);
//    }
//    return Py::new_reference_to(PythonObject);
//}
/// @endcond

// explicit template instantiation
template class FemExport FeaturePythonT<Fem::FemAnalysis>;
}  // namespace App

// ---------------------------------------------------------

namespace App
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Fem::FeaturePython, Fem::DocumentObject)
template<>
const char* Fem::FeaturePython::getViewProviderName() const
{
    return "Gui::ViewProviderFeaturePython";
}
template<>
PyObject* Fem::FeaturePython::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new App::FeaturePythonPyT<App::DocumentObjectPy>(this), true);
    }
    return Py::new_reference_to(PythonObject);
}
// explicit template instantiation
template class FemExport FeaturePythonT<Fem::DocumentObject>;
/// @endcond
}  // namespace App
