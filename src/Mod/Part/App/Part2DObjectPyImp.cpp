#include "Part2DObject.h"
#include "OCCError.h"

// inclusion of the generated files (generated out of Part2DObjectPy.xml)
#include "Part2DObjectPy.h"
#include "Part2DObjectPy.cpp"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string Part2DObjectPy::representation() const
{
    return {"<Part::Part2DObject>"};
}

PyObject* Part2DObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int Part2DObjectPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
