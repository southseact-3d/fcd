# include <sstream>


#include "DrawView.h"
#include "DrawViewAnnotation.h"

// inclusion of the generated files
#include <Mod/TechDraw/App/DrawViewPy.h>
#include <Mod/TechDraw/App/DrawViewAnnotationPy.h>
#include <Mod/TechDraw/App/DrawViewAnnotationPy.cpp>


using namespace TechDraw;

// returns a string which represents the object e.g. when printed in python
std::string DrawViewAnnotationPy::representation() const
{
    return std::string("<DrawViewAnnotation object>");
}

PyObject *DrawViewAnnotationPy::getCustomAttributes(const char* ) const
{
    return nullptr;
}

int DrawViewAnnotationPy::setCustomAttributes(const char* , PyObject *)
{
    return 0;
}
