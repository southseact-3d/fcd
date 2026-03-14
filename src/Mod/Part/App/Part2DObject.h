#ifndef PART_PART2DOBJECT_H
#define PART_PART2DOBJECT_H

#include <App/PropertyStandard.h>
#include <Base/Axis.h>

#include <Mod/Part/PartGlobal.h>

#include "AttachExtension.h"


class TopoDS_Face;

namespace Part
{
class Geometry;

/** 2D Shape
 * This is a specialized version of the PartShape for use with
 * flat (2D) geometry. The Z direction has always to be 0.
 * The position and orientation of the Plane this 2D geometry is
 * referenced is defined by the Placement property. It also
 * has a link to a supporting Face which defines the position
 * in space where it is located. If the support is changed the
 * static method positionBySupport() is used to calculate a
 * new position for the Part2DObject.
 * This object can be used stand alone or for constraint
 * geometry as its descend Sketcher::SketchObject .
 */

class PartExport Part2DObject: public Part::Feature, public Part::AttachExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(Part::Part2DObject);

public:
    Part2DObject();

    void transformPlacement(const Base::Placement& transform) override;

    /// returns the number of construction lines (to be used as axes)
    virtual int getAxisCount() const;
    /// retrieves an axis iterating through the construction lines of the sketch (indices start at 0)
    virtual Base::Axis getAxis(int axId) const;
    /// verify and accept the assigned geometry
    virtual void acceptGeometry();

    /** calculate the points where a curve with index geometryIndex should be trimmed
     * with respect to the rest of the curves contained in the list geomlist
     * and a picked point. The outputs intersect1 and intersect2 specify the
     * tightest boundaries for trimming around the picked point and the
     * indexes geometryIndex1 and geometryIndex2 specify the corresponding curves that intersect
     * the curve geometryIndex.
     *
     * If intersection is found, the associated geometryIndex1 or geometryIndex2 returns -1.
     */
    static bool seekTrimPoints(
        const std::vector<Geometry*>& geomlist,
        int geometryIndex,
        const Base::Vector3d& point,
        int& geometryIndex1,
        Base::Vector3d& intersect1,
        int& geometryIndex2,
        Base::Vector3d& intersect2
    );

    static const int H_Axis;
    static const int V_Axis;
    static const int N_Axis;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProvider2DObject";
    }
    //@}

    void Restore(Base::XMLReader& reader) override;
};

using Part2DObjectPython = App::FeaturePythonT<Part2DObject>;

}  // namespace Part


#endif  // PART_PART2DOBJECT_H
