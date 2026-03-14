#ifndef PART_FEATUREPARTPOLYGON_H
#define PART_FEATUREPARTPOLYGON_H

#include <App/PropertyGeo.h>

#include "PartFeature.h"


namespace Part
{

class Polygon: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Polygon);

public:
    Polygon();
    ~Polygon() override;

    App::PropertyVectorList Nodes;
    App::PropertyBool Close;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    //@}
};

}  // namespace Part

#endif  // PART_FEATUREPARTPOLYGON_H
