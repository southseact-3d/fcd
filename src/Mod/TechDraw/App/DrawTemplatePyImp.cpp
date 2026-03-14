# include <sstream>


#include "DrawTemplate.h"
// inclusion of the generated files (generated out of DrawTemplateSFPy.xml)
#include <Mod/TechDraw/App/DrawTemplatePy.h>
#include <Mod/TechDraw/App/DrawTemplatePy.cpp>


using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string DrawTemplatePy::representation() const
{
    return "<TechDraw::DrawTemplate>";
}

PyObject *DrawTemplatePy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DrawTemplatePy::setCustomAttributes(const char* attr, PyObject* obj)
{
    // search in PropertyList
    App::Property *prop = getDrawTemplatePtr()->getPropertyByName(attr);
    if (!prop) {
        return 0;
    }

    // Read-only attributes must not be set over its Python interface
    short Type =  getDrawTemplatePtr()->getPropertyType(prop);
    if (Type & App::Prop_ReadOnly) {
        std::stringstream s;
        s << "Object attribute '" << attr << "' is read-only";
        throw Py::AttributeError(s.str());
    }

    prop->setPyObject(obj);
    return 1;
}
