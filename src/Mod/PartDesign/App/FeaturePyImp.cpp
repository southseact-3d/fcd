// inclusion of the generated files (generated out of FeaturePy.xml)
#include "FeaturePy.h"
#include "FeaturePy.cpp"

using namespace PartDesign;

// returns a string which represent the object e.g. when printed in python
std::string FeaturePy::representation() const
{
    App::DocumentObject* object = this->getFeaturePtr();
    std::stringstream str;
    str << "<" << object->getTypeId().getName() << ">";
    return str.str();
}

PyObject* FeaturePy::getCustomAttributes(const char*) const
{
    return nullptr;
}

int FeaturePy::setCustomAttributes(const char*, PyObject*)
{
    return 0;
}

PyObject* FeaturePy::getBaseObject(PyObject* /*args*/)
{
    App::DocumentObject* base = getFeaturePtr()->getBaseObject();
    if (base) {
        return base->getPyObject();
    }
    else {
        return Py::new_reference_to(Py::None());
    }
}
