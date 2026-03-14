#include "DocumentObjectGroup.h"
#include "DocumentObjectGroupPy.h"
#include "FeaturePythonPyImp.h"

using namespace App;

PROPERTY_SOURCE_WITH_EXTENSIONS(App::DocumentObjectGroup, App::DocumentObject)

DocumentObjectGroup::DocumentObjectGroup()
    : DocumentObject()
    , GroupExtension()
{

    GroupExtension::initExtension(this);
    _GroupTouched.setStatus(App::Property::Output, true);
}

DocumentObjectGroup::~DocumentObjectGroup() = default;

PyObject* DocumentObjectGroup::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new DocumentObjectGroupPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}


// Python feature ---------------------------------------------------------

namespace App
{

/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(App::DocumentObjectGroupPython, App::DocumentObjectGroup)
template<>
const char* App::DocumentObjectGroupPython::getViewProviderName() const
{
    return "Gui::ViewProviderDocumentObjectGroupPython";
}
template<>
PyObject* App::DocumentObjectGroupPython::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new FeaturePythonPyT<App::DocumentObjectGroupPy>(this), true);
    }
    return Py::new_reference_to(PythonObject);
}
/// @endcond

// explicit template instantiation
template class AppExport FeaturePythonT<App::DocumentObjectGroup>;
}  // namespace App
