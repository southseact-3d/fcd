// inclusion of the generated files (generated out of GeoFeatureGroupExtensionPy.xml)
#include "GeoFeatureGroupExtensionPy.h"
#include "GeoFeatureGroupExtensionPy.cpp"

using namespace App;

// returns a string which represents the object e.g. when printed in python
std::string GeoFeatureGroupExtensionPy::representation() const
{
    return {"<GeoFeatureGroup object>"};
}

PyObject* GeoFeatureGroupExtensionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int GeoFeatureGroupExtensionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
