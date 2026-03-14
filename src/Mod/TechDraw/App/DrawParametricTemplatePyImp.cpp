# include <sstream>


#include "DrawParametricTemplate.h"
// inclusion of the generated files (generated out of DrawParametricTemplateSFPy.xml)
#include <Mod/TechDraw/App/DrawParametricTemplatePy.h>
#include <Mod/TechDraw/App/DrawParametricTemplatePy.cpp>


using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string DrawParametricTemplatePy::representation(void) const
{
    return "<TechDraw::DrawParametricTemplate>";
}

PyObject *DrawParametricTemplatePy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DrawParametricTemplatePy::setCustomAttributes(const char* attr, PyObject* obj)
{
    // search in PropertyList
    App::Property *prop = getDrawParametricTemplatePtr()->getPropertyByName(attr);
    if (!prop) {
        return 0;
    }

    // Read-only attributes must not be set over its Python interface
    short Type =  getDrawParametricTemplatePtr()->getPropertyType(prop);
    if (Type & App::Prop_ReadOnly) {
        std::stringstream s;
        s << "Object attribute '" << attr << "' is read-only";
        throw Py::AttributeError(s.str());
    }

    prop->setPyObject(obj);
    return 1;
}

PyObject* DrawParametricTemplatePy::drawLine(PyObject *args)
{
    //PyObject *pcObj;
    double x1, y1;
    double x2, y2;

    if (!PyArg_ParseTuple(args, "dddd", &x1, &y1, &x2, &y2))
        return nullptr;

    getDrawParametricTemplatePtr()->drawLine(x1, y1, x2, y2);

    Py_Return;
}

Py::Long DrawParametricTemplatePy::getGeometryCount(void) const
{
    int size = getDrawParametricTemplatePtr()->getGeometry().size();
    return Py::Long(size);
}
