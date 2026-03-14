#include <Base/Tools.h>

#include "Extension.h"
#include "SuppressibleExtension.h"
#include "SuppressibleExtensionPy.h"


namespace App
{

EXTENSION_PROPERTY_SOURCE(App::SuppressibleExtension, App::DocumentObjectExtension)


EXTENSION_PROPERTY_SOURCE_TEMPLATE(App::SuppressibleExtensionPython, App::SuppressibleExtension)

// explicit template instantiation
template class AppExport ExtensionPythonT<SuppressibleExtensionPythonT<SuppressibleExtension>>;


SuppressibleExtension::SuppressibleExtension()
{
    initExtensionType(SuppressibleExtension::getExtensionClassTypeId());
    EXTENSION_ADD_PROPERTY_TYPE(Suppressed,
                                (false),
                                "Base",
                                PropertyType(Prop_None),
                                "Is object suppressed");
}

SuppressibleExtension::~SuppressibleExtension() = default;

PyObject* SuppressibleExtension::getExtensionPyObject()
{

    if (ExtensionPythonObject.is(Py::_None())) {
        // ref counter is set to 1
        auto ext = new SuppressibleExtensionPy(this);
        ExtensionPythonObject = Py::Object(ext, true);
    }
    return Py::new_reference_to(ExtensionPythonObject);
}

}  // namespace App
