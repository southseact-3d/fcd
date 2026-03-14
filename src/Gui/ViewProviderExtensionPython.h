#ifndef GUI_VIEWPROVIDEREXTENSIONPYTHON_H
#define GUI_VIEWPROVIDEREXTENSIONPYTHON_H

#include "ViewProviderExtension.h"
#include <App/PropertyPythonObject.h>

namespace Gui
{

/**
 * Generic Python extension class which allows one to behave every extension
 * derived class as Python extension -- simply by subclassing.
 */
template<class ExtensionT>
class ViewProviderExtensionPythonT: public ExtensionT  // NOLINT
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderExtensionPythonT<ExtensionT>);

public:
    using Inherited = ExtensionT;

    ViewProviderExtensionPythonT()
    {
        ExtensionT::m_isPythonExtension = true;
        ExtensionT::initExtensionType(ViewProviderExtensionPythonT::getExtensionClassTypeId());
    }
    ~ViewProviderExtensionPythonT() override = default;

    ViewProviderExtensionPythonT(const ViewProviderExtensionPythonT&) = delete;
    ViewProviderExtensionPythonT(ViewProviderExtensionPythonT&&) = delete;
    ViewProviderExtensionPythonT& operator=(const ViewProviderExtensionPythonT&) = delete;
    ViewProviderExtensionPythonT& operator=(ViewProviderExtensionPythonT&&) = delete;
};

using ViewProviderExtensionPython = ViewProviderExtensionPythonT<Gui::ViewProviderExtension>;

}  // namespace Gui

#endif  // GUI_VIEWPROVIDEREXTENSIONPYTHON_H
