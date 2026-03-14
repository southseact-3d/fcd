#ifndef SUPPRESSIBLEEXTENSION_H
#define SUPPRESSIBLEEXTENSION_H

#include <App/DocumentObject.h>
#include <App/DocumentObjectExtension.h>
#include <App/ExtensionPython.h>

namespace App
{
class SuppressibleExtensionPy;

class AppExport SuppressibleExtension: public DocumentObjectExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(App::SuppressibleExtension);
    using inherited = DocumentObjectExtension;

public:
    /// Constructor
    SuppressibleExtension();
    ~SuppressibleExtension() override;

    PyObject* getExtensionPyObject() override;

    /// Properties
    PropertyBool Suppressed;
};

template<typename ExtensionT>
class SuppressibleExtensionPythonT: public ExtensionT
{

public:
    SuppressibleExtensionPythonT() = default;
    ~SuppressibleExtensionPythonT() override = default;
};

using SuppressibleExtensionPython =
    ExtensionPythonT<SuppressibleExtensionPythonT<SuppressibleExtension>>;

}  // namespace App

#endif  // SUPPRESSIBLEEXTENSION_H
