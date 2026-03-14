// inclusion of the generated files (generated out of AssemblyLink.xml)
#include "AssemblyLinkPy.h"
#include "AssemblyLinkPy.cpp"

using namespace Assembly;

// returns a string which represents the object e.g. when printed in python
std::string AssemblyLinkPy::representation() const
{
    return {"<Assembly link>"};
}

PyObject* AssemblyLinkPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int AssemblyLinkPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}

Py::List AssemblyLinkPy::getJoints() const
{
    Py::List ret;
    std::vector<App::DocumentObject*> list = getAssemblyLinkPtr()->getJoints();

    for (auto It : list) {
        ret.append(Py::Object(It->getPyObject(), true));
    }

    return ret;
}
