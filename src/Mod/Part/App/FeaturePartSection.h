#ifndef PART_FEATUREPARTSECTION_H
#define PART_FEATUREPARTSECTION_H

#include <App/PropertyStandard.h>

#include "FeaturePartBoolean.h"


namespace Part
{

class Section: public Boolean
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Section);

public:
    Section();

    App::PropertyBool Approximation;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    short mustExecute() const override;

protected:
    BRepAlgoAPI_BooleanOperation* makeOperation(const TopoDS_Shape&, const TopoDS_Shape&) const override;
    const char* opCode() const override;
    //@}
};

}  // namespace Part

#endif  // PART_FEATUREPARTSECTION_H
