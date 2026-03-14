#include <cassert>
#include <algorithm>


#include "ViewProviderExtension.h"
#include "ViewProviderExtensionPy.h"
#include "ViewProviderExtensionPython.h"
#include "ViewProviderDocumentObject.h"

using namespace Gui;

EXTENSION_PROPERTY_SOURCE(Gui::ViewProviderExtension, App::Extension)

ViewProviderExtension::ViewProviderExtension()
{
    initExtensionType(Gui::ViewProviderExtension::getExtensionClassTypeId());
}

ViewProviderExtension::~ViewProviderExtension() = default;

const ViewProviderDocumentObject* ViewProviderExtension::getExtendedViewProvider() const
{

    assert(getExtendedContainer()->isDerivedFrom<ViewProviderDocumentObject>());
    return static_cast<const ViewProviderDocumentObject*>(getExtendedContainer());
}

ViewProviderDocumentObject* ViewProviderExtension::getExtendedViewProvider()
{

    assert(getExtendedContainer()->isDerivedFrom<ViewProviderDocumentObject>());
    return static_cast<ViewProviderDocumentObject*>(getExtendedContainer());
}

void ViewProviderExtension::extensionUpdateData(const App::Property*)
{}

PyObject* ViewProviderExtension::getExtensionPyObject()
{

    if (ExtensionPythonObject.is(Py::_None())) {
        // ref counter is set to 1
        auto ext = new ViewProviderExtensionPy(this);
        ExtensionPythonObject = Py::asObject(ext);
    }
    return Py::new_reference_to(ExtensionPythonObject);
}

namespace Gui
{
EXTENSION_PROPERTY_SOURCE_TEMPLATE(Gui::ViewProviderExtensionPython, Gui::ViewProviderExtension)

// explicit template instantiation
template class GuiExport ViewProviderExtensionPythonT<ViewProviderExtension>;
}  // namespace Gui
