#include "BodyBase.h"
// inclusion of the generated files (generated out of ItemPy.xml)
#include "BodyBasePy.h"
#include "BodyBasePy.cpp"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string BodyBasePy::representation() const
{
    return {"<body object>"};
}

PyObject* BodyBasePy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int BodyBasePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
