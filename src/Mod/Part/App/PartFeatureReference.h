#ifndef PART_FeatureReference_H
#define PART_FeatureReference_H

#include <App/GeoFeature.h>

#include <Mod/Part/PartGlobal.h>

#include "PropertyTopoShape.h"


namespace Part
{

class PartFeaturePy;

/** Base class of all shape feature classes in FreeCAD
 */
class PartExport FeatureReference: public App::GeoFeature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::FeatureReference);

public:
    /// Constructor
    FeatureReference();
    ~FeatureReference() override;

    App::PropertyLink Reference;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    //@}

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderPartReference";
    }

protected:
    TopLoc_Location getLocation() const;
};


}  // namespace Part


#endif  // PART_FeatureReference_H
