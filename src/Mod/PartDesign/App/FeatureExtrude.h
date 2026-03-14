#ifndef PARTDESIGN_FEATURE_EXTRUDE_H
#define PARTDESIGN_FEATURE_EXTRUDE_H

#include <App/PropertyStandard.h>
#include <App/PropertyUnits.h>
#include "FeatureSketchBased.h"

class gp_Dir;
class TopoDS_Face;
class TopoDS_Shape;
class TopLoc_Location;

namespace PartDesign
{

class PartDesignExport FeatureExtrude: public ProfileBased
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::FeatureExtrude);

public:
    FeatureExtrude();

    App::PropertyEnumeration SideType;
    App::PropertyEnumeration Type;
    App::PropertyEnumeration Type2;
    App::PropertyLength Length;
    App::PropertyLength Length2;
    App::PropertyAngle TaperAngle;
    App::PropertyAngle TaperAngle2;
    App::PropertyBool UseCustomVector;
    App::PropertyVector Direction;
    App::PropertyBool AlongSketchNormal;
    App::PropertyLength Offset;
    App::PropertyLength Offset2;
    App::PropertyLinkSub ReferenceAxis;

    static App::PropertyQuantityConstraint::Constraints signedLengthConstraint;
    static double maxAngle;
    static App::PropertyAngle::Constraints floatAngle;

    /** @name methods override feature */
    //@{
    short mustExecute() const override;
    void setupObject() override;

    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderExtrude";
    }
    //@}

    static const char* SideTypesEnums[];

protected:
    void onDocumentRestored() override;
    Base::Vector3d computeDirection(const Base::Vector3d& sketchVector, bool inverse);
    bool hasTaperedAngle() const;
    void onChanged(const App::Property* prop) override;


    /// Options for buildExtrusion()
    enum class ExtrudeOption
    {
        MakeFace = 1,
        MakeFuse = 2,
        LegacyPocket = 4,
        InverseDirection = 8,
    };

    using ExtrudeOptions = Base::Flags<ExtrudeOption>;

    App::DocumentObjectExecReturn* buildExtrusion(ExtrudeOptions options);

    /**
     * generate an open shell from a given shape
     * by removing the farthest face from the sketchshape in the direction
     * if farthest is nearest (circular) then return the initial shape
     */
    TopoShape makeShellFromUpToShape(TopoShape shape, TopoShape sketchshape, gp_Dir& dir);

    /**
     * Disables settings that are not valid for the current method
     */
    void updateProperties();

    TopoShape generateSingleExtrusionSide(
        const TopoShape& sketchShape,  // The base sketch for this side (global CS)
        const std::string& method,
        double length,
        double taperAngleDeg,
        App::PropertyLinkSub& upToFacePropHandle,       // e.g., &UpToFace or &UpToFace2
        App::PropertyLinkSubList& upToShapePropHandle,  // e.g., &UpToShape or &UpToShape2
        gp_Dir dir,
        double offsetVal,
        bool makeFace,
        const TopoShape& base,      // The base shape for context (global CS)
        TopLoc_Location& invObjLoc  // MUST be passed. Cannot be re-accessed, see #26677
    );
};

}  // namespace PartDesign

ENABLE_BITMASK_OPERATORS(PartDesign::FeatureExtrude::ExtrudeOption)

#endif  // PARTDESIGN_FEATURE_EXTRUDE_H
