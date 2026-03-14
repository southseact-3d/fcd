#ifndef PART_FEATURESCALE_H
#define PART_FEATURESCALE_H

#include <App/PropertyStandard.h>
#include <App/PropertyUnits.h>

#include <Mod/Part/PartGlobal.h>

#include "FaceMakerCheese.h"
#include "PartFeature.h"


namespace Part
{

class PartExport Scale: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Scale);

public:
    Scale();

    App::PropertyLink Base;
    App::PropertyBool Uniform;
    App::PropertyFloat UniformScale;
    App::PropertyFloat XScale;
    App::PropertyFloat YScale;
    App::PropertyFloat ZScale;

    /**
     * @brief The ScaleParameters struct is supposed to be filled with final
     * scale parameters and be passed to scaleShape.
     */
    struct ScaleParameters
    {
        bool uniform {true};
        double uniformScale {1.0};
        double XScale {1.0};
        double YScale {1.0};
        double ZScale {1.0};
    };

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderScale";
    }
    //@}
    Scale::ScaleParameters computeFinalParameters();

    /**
     * @brief scaleShape powers the extrusion feature.
     * @param source: the shape to be scaled
     * @param params: scale parameters
     * @return result of scaling
     */
    static TopoShape scaleShape(const TopoShape& source, const ScaleParameters& params);
    static TopoShape uniformScale(const TopoShape& source, const double& factor);
    static TopoShape nonuniformScale(const TopoShape& source, const Scale::ScaleParameters& params);
};

}  // namespace Part


#endif  // PART_FEATURESCALE_H
