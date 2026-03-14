// inclusion of the generated files (generated out of SimulationGroup.xml)
#include "SimulationGroupPy.h"
#include "SimulationGroupPy.cpp"

using namespace Assembly;

// returns a string which represents the object e.g. when printed in python
std::string SimulationGroupPy::representation() const
{
    return {"<Simulation Group>"};
}

PyObject* SimulationGroupPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int SimulationGroupPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
