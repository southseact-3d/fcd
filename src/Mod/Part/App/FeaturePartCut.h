#ifndef PART_FEATUREPARTCUT_H
#define PART_FEATUREPARTCUT_H

#include "FeaturePartBoolean.h"


namespace Part
{

class Cut: public Boolean
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Cut);

public:
    Cut();

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
protected:
    BRepAlgoAPI_BooleanOperation* makeOperation(const TopoDS_Shape&, const TopoDS_Shape&) const override;
    const char* opCode() const override;
    //@}
};

}  // namespace Part

#endif  // PART_FEATUREPARTCUT_H
