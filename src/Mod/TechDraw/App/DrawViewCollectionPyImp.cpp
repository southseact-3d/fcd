#include "DrawViewCollection.h"
// inclusion of the generated files (generated out of DrawViewCollectionPy.xml)
#include <Mod/TechDraw/App/DrawViewCollectionPy.h>
#include <Mod/TechDraw/App/DrawViewCollectionPy.cpp>


using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string DrawViewCollectionPy::representation(void) const
{
    return std::string("<DrawViewCollection object>");
}
PyObject* DrawViewCollectionPy::addView(PyObject* args)
{
    PyObject *pcDocObj;
    if (!PyArg_ParseTuple(args, "O!", &(TechDraw::DrawViewPy::Type), &pcDocObj)) {
        return nullptr;
    }

    DrawViewCollection* collect = getDrawViewCollectionPtr();
    DrawViewPy* pyView = static_cast<TechDraw::DrawViewPy*>(pcDocObj);
    DrawView* view = pyView->getDrawViewPtr();                 //get DrawView for pyView

    int i = collect->addView(view);

    return PyLong_FromLong(i);
}

PyObject* DrawViewCollectionPy::removeView(PyObject* args)
{
    PyObject *pcDocObj;
    if (!PyArg_ParseTuple(args, "O!", &(TechDraw::DrawViewPy::Type), &pcDocObj)) {
        return nullptr;
    }

    DrawViewCollection* collect = getDrawViewCollectionPtr();
    DrawViewPy* pyView = static_cast<TechDraw::DrawViewPy*>(pcDocObj);
    DrawView* view = pyView->getDrawViewPtr();                 //get DrawView for pyView

    int i = collect->removeView(view);

    return PyLong_FromLong(i);
}


PyObject *DrawViewCollectionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DrawViewCollectionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
