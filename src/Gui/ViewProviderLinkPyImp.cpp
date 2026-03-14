#include <sstream>

#include <Base/PlacementPy.h>

// generated out of ViewProviderLink.pyi
#include "ViewProviderLinkPy.h"
#include "ViewProviderLinkPy.cpp"

using namespace Gui;

// returns a string which represents the object e.g. when printed in python
std::string ViewProviderLinkPy::representation() const
{
    std::stringstream str;
    str << "<ViewProviderLink at " << getViewProviderLinkPtr() << ">";

    return str.str();
}

Py::Object ViewProviderLinkPy::getDraggingPlacement() const
{
    return Py::asObject(
        new Base::PlacementPy(new Base::Placement(getViewProviderLinkPtr()->getDraggerPlacement()))
    );
}

void ViewProviderLinkPy::setDraggingPlacement(Py::Object arg)
{
    if (!PyObject_TypeCheck(arg.ptr(), &Base::PlacementPy::Type)) {
        throw Py::TypeError("expects a placement");
    }
    getViewProviderLinkPtr()->setDraggerPlacement(
        *static_cast<Base::PlacementPy*>(arg.ptr())->getPlacementPtr()
    );
}

Py::Object ViewProviderLinkPy::getLinkView() const
{
    return Py::Object(getViewProviderLinkPtr()->getPyLinkView(), true);
}

PyObject* ViewProviderLinkPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ViewProviderLinkPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
