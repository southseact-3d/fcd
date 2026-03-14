#ifndef PARTDESIGN_FeatureSolid_H
#define PARTDESIGN_FeatureSolid_H

#include "Feature.h"


/// Base class of all additive features in PartDesign
namespace PartDesign
{

/** PartDesign feature
 *   Base class of all PartDesign features.
 *   This kind of features only produce solids or fail.
 */
class PartDesignExport Solid: public Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Solid);

public:
    Solid();

protected:
};

}  // namespace PartDesign


#endif  // PARTDESIGN_Feature_H
