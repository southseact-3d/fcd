// inclusion of the generated files (generated out of RobotObjectPy.xml)
#include "RobotObjectPy.h"
#include "RobotObjectPy.cpp"
// clang-format on


using namespace Robot;

// returns a string which represents the object e.g. when printed in python
std::string RobotObjectPy::representation() const
{
    return {"<RobotObject object>"};
}


PyObject* RobotObjectPy::getRobot(PyObject* /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return nullptr;
}


PyObject* RobotObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int RobotObjectPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
