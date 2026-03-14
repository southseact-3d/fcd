#include <App/DocumentObjectPy.h>

#include "FeaturePath.h"


using namespace Path;

PROPERTY_SOURCE(Path::Feature, App::GeoFeature)


Feature::Feature()
{
    ADD_PROPERTY_TYPE(Path, (Path::Toolpath()), "Base", App::Prop_None, "The path data of this feature");
}

Feature::~Feature()
{}

short Feature::mustExecute() const
{
    return App::GeoFeature::mustExecute();
}

PyObject* Feature::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new App::DocumentObjectPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}

void Feature::onChanged(const App::Property* prop)
{
    App::GeoFeature::onChanged(prop);
}

// Python Path feature ---------------------------------------------------------

namespace App
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Path::FeaturePython, Path::Feature)
template<>
const char* Path::FeaturePython::getViewProviderName() const
{
    return "PathGui::ViewProviderPathPython";
}
/// @endcond

// explicit template instantiation
template class PathExport FeaturePythonT<Path::Feature>;
}  // namespace App
