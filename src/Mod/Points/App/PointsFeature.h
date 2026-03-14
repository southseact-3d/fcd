#ifndef POINTS_FEATURE_H
#define POINTS_FEATURE_H

#include <App/FeatureCustom.h>
#include <App/FeaturePython.h>
#include <App/GeoFeature.h>
#include <App/PropertyGeo.h>

#include "Points.h"
#include "PropertyPointKernel.h"


namespace Base
{
class Writer;
}

namespace App
{
class Color;
}

namespace Points
{
class Property;
class PointsFeaturePy;

/** Base class of all Points feature classes in FreeCAD.
 * This class holds an PointsKernel object.
 */
class PointsExport Feature: public App::GeoFeature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Points::Feature);

public:
    /// Constructor
    Feature();

    /** @name methods override Feature */
    //@{
    void Restore(Base::XMLReader& reader) override;
    void RestoreDocFile(Base::Reader& reader) override;
    short mustExecute() const override;
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PointsGui::ViewProviderScattered";
    }

    const App::PropertyComplexGeoData* getPropertyOfGeometry() const override
    {
        return &Points;
    }

protected:
    void onChanged(const App::Property* prop) override;
    //@}

public:
    PropertyPointKernel Points; /**< The point kernel property. */
};

using FeatureCustom = App::FeatureCustomT<Feature>;
using FeaturePython = App::FeaturePythonT<Feature>;

}  // namespace Points


#endif
