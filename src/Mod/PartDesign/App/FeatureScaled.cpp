#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>


#include "FeatureScaled.h"
#include "FeatureAddSub.h"


using namespace PartDesign;

namespace PartDesign
{


PROPERTY_SOURCE(PartDesign::Scaled, PartDesign::Transformed)

Scaled::Scaled()
{
    ADD_PROPERTY(Factor, (2.0));
    ADD_PROPERTY(Occurrences, (2));
}

short Scaled::mustExecute() const
{
    if (Factor.isTouched() || Occurrences.isTouched()) {
        return 1;
    }
    return Transformed::mustExecute();
}

const std::list<gp_Trsf> Scaled::getTransformations(const std::vector<App::DocumentObject*> originals)
{
    double const factor = Factor.getValue();
    if (factor < Precision::Confusion()) {
        throw Base::ValueError("Scaling factor too small");
    }
    int const occurrences = Occurrences.getValue();
    if (occurrences < 2) {
        throw Base::ValueError("At least two occurrences required");
    }

    double f = (factor - 1.0) / double(occurrences - 1);

    gp_Pnt cog;
    if (!originals.empty()) {
        // Find centre of gravity of first original
        // FIXME: This method will NOT give the expected result for more than one original!
        if (auto feature = freecad_cast<PartDesign::FeatureAddSub*>(originals.front())) {
            TopoDS_Shape original = feature->AddSubShape.getShape().getShape();

            GProp_GProps props;
            BRepGProp::VolumeProperties(original, props);
            cog = props.CentreOfMass();
        }
    }

    // Note: The original feature is NOT included in the list of transformations! Therefore
    // we start with occurrence number 1, not number 0
    std::list<gp_Trsf> transformations;
    gp_Trsf trans;
    transformations.push_back(trans);  // identity transformation

    for (int i = 1; i < occurrences; i++) {
        trans.SetScale(cog, 1.0 + double(i) * f);
        transformations.push_back(trans);
    }

    return transformations;
}

}  // namespace PartDesign
