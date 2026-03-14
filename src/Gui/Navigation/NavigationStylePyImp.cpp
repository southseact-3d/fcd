// generated out of NavigationStyle.pyi
#include "Navigation/NavigationStylePy.h"
#include "Navigation/NavigationStylePy.cpp"

using namespace Gui;

/** @class NavigationStylePy
 * The NavigationStyle Python class provides additional methods for manipulation of
 * navigation style objects.
 * @see NavigationStyle
 */

// returns a string which represent the object e.g. when printed in python
std::string NavigationStylePy::representation() const
{
    return {"<NavigationStyle object>"};
}

PyObject* NavigationStylePy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int NavigationStylePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
