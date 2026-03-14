#include "CustomFeature.h"


using namespace Part;


PROPERTY_SOURCE(Part::CustomFeature, Part::Feature)


CustomFeature::CustomFeature() = default;

CustomFeature::~CustomFeature() = default;

short CustomFeature::mustExecute() const
{
    return Part::Feature::mustExecute();
}

App::DocumentObjectExecReturn* CustomFeature::execute()
{
    return App::DocumentObject::StdReturn;
}

// ---------------------------------------------------------

namespace App
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Part::CustomFeaturePython, Part::CustomFeature)
template<>
const char* Part::CustomFeaturePython::getViewProviderName() const
{
    return "PartGui::ViewProviderCustomPython";
}
/// @endcond

// explicit template instantiation
template class PartExport FeaturePythonT<Part::CustomFeature>;
}  // namespace App
