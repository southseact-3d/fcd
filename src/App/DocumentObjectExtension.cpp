#include "DocumentObjectExtension.h"
#include "DocumentObjectExtensionPy.h"
#include "DocumentObject.h"
#include "ExtensionContainer.h"

using namespace App;

EXTENSION_PROPERTY_SOURCE(App::DocumentObjectExtension, App::Extension)

DocumentObjectExtension::DocumentObjectExtension()
{
    initExtensionType(App::DocumentObjectExtension::getExtensionClassTypeId());
}

DocumentObjectExtension::~DocumentObjectExtension() = default;

short int DocumentObjectExtension::extensionMustExecute()
{

    return 0;
}

App::DocumentObjectExecReturn* DocumentObjectExtension::extensionExecute()
{

    return App::DocumentObject::StdReturn;
}

void DocumentObjectExtension::onExtendedSettingDocument()
{}

void DocumentObjectExtension::onExtendedDocumentRestored()
{}

void DocumentObjectExtension::onExtendedSetupObject()
{}

void DocumentObjectExtension::onExtendedUnsetupObject()
{}

PyObject* DocumentObjectExtension::getExtensionPyObject()
{

    if (ExtensionPythonObject.is(Py::_None())) {
        // ref counter is set to 1
        ExtensionPythonObject = Py::Object(new DocumentObjectExtensionPy(this), true);
    }
    return Py::new_reference_to(ExtensionPythonObject);
}

const DocumentObject* DocumentObjectExtension::getExtendedObject() const
{

    assert(getExtendedContainer()->isDerivedFrom<DocumentObject>());
    return static_cast<const DocumentObject*>(getExtendedContainer());
}

DocumentObject* DocumentObjectExtension::getExtendedObject()
{

    assert(getExtendedContainer()->isDerivedFrom<DocumentObject>());
    return static_cast<DocumentObject*>(getExtendedContainer());
}

bool DocumentObjectExtension::extensionGetSubObject(DocumentObject*&,
                                                    const char*,
                                                    PyObject**,
                                                    Base::Matrix4D*,
                                                    bool,
                                                    int) const
{
    return false;
}

bool DocumentObjectExtension::extensionGetSubObjects(std::vector<std::string>&, int) const
{
    return false;
}

bool DocumentObjectExtension::extensionGetLinkedObject(DocumentObject*&,
                                                       bool,
                                                       Base::Matrix4D*,
                                                       bool,
                                                       int) const
{
    return false;
}
