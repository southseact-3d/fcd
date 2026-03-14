#include <vector>


#include "PointsFeature.h"


using namespace Points;

//===========================================================================
// Feature
//===========================================================================

PROPERTY_SOURCE(Points::Feature, App::GeoFeature)

Feature::Feature()
{
    ADD_PROPERTY(Points, (PointKernel()));
}

short Feature::mustExecute() const
{
    return 0;
}

App::DocumentObjectExecReturn* Feature::execute()
{
    this->Points.touch();
    return App::DocumentObject::StdReturn;
}

void Feature::Restore(Base::XMLReader& reader)
{
    GeoFeature::Restore(reader);
}

void Feature::RestoreDocFile(Base::Reader& reader)
{
    // This gets only invoked if a points file has been added from Restore()
    Points.RestoreDocFile(reader);
}

void Feature::onChanged(const App::Property* prop)
{
    // if the placement has changed apply the change to the point data as well
    if (prop == &this->Placement) {
        this->Points.setTransform(this->Placement.getValue().toMatrix());
    }
    // if the point data has changed check and adjust the transformation as well
    else if (prop == &this->Points) {
        try {
            Base::Placement p;
            p.fromMatrix(this->Points.getTransform());
            if (p != this->Placement.getValue()) {
                this->Placement.setValue(p);
            }
        }
        catch (const Base::ValueError&) {
        }
    }

    GeoFeature::onChanged(prop);
}

// ---------------------------------------------------------

namespace App
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Points::FeatureCustom, Points::Feature)
/// @endcond

// explicit template instantiation
template class PointsExport FeatureCustomT<Points::Feature>;
}  // namespace App

// ---------------------------------------------------------

namespace App
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Points::FeaturePython, Points::Feature)
template<>
const char* Points::FeaturePython::getViewProviderName() const
{
    return "PointsGui::ViewProviderPython";
}
/// @endcond

// explicit template instantiation
template class PointsExport FeaturePythonT<Points::Feature>;
}  // namespace App
