//this file originally part of TechDraw workbench
//migrated to TechDraw workbench 2022-01-26 by Wandererfan

#ifndef TECHDRAW_FEATUREPROJECTION
#define TECHDRAW_FEATUREPROJECTION

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <App/DocumentObject.h>
#include <Mod/Part/App/PartFeature.h>


namespace TechDraw
{

/** Base class of all View Features in the drawing module
 */
class TechDrawExport FeatureProjection : public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::FeatureProjection);

public:
    /// Constructor
    FeatureProjection();
    ~FeatureProjection() override;

    App::PropertyLink   Source;
    App::PropertyVector Direction;
    App::PropertyBool   VCompound;
    App::PropertyBool   Rg1LineVCompound;
    App::PropertyBool   RgNLineVCompound;
    App::PropertyBool   OutLineVCompound;
    App::PropertyBool   IsoLineVCompound;
    App::PropertyBool   HCompound;
    App::PropertyBool   Rg1LineHCompound;
    App::PropertyBool   RgNLineHCompound;
    App::PropertyBool   OutLineHCompound;
    App::PropertyBool   IsoLineHCompound;

    /** @name methods override feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute() override;
    //@}
};

} //namespace TechDraw



#endif // TECHDRAW_FEATUREPROJECTION
