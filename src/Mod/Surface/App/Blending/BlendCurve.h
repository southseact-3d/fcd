#ifndef SURFACE_BLEND_CURVE_H
#define SURFACE_BLEND_CURVE_H

#include <Geom_BezierCurve.hxx>
#include <Mod/Surface/App/Blending/BlendPoint.h>
#include <Mod/Surface/SurfaceGlobal.h>

namespace Surface
{
/*!
 * Create a BezierCurve interpolating a list of BlendPoints
 */
class SurfaceExport BlendCurve
{
public:
    std::vector<BlendPoint> blendPoints;

    BlendCurve() = default;
    /*!
     *  Constructor
     *\param std::vector<BlendPoint>
     */
    explicit BlendCurve(const std::vector<BlendPoint>& blendPointsList);
    ~BlendCurve() = default;
    /*!
     *  Perform the interpolate algorithm
     *\return the BezierCurve
     */
    Handle(Geom_BezierCurve) compute();
    /*!
     *  Set the size of the first derivative of a BlendPoint
     *\param int index of the BlendPoint to modify
     *\param double new size
     *\param bool interpret new size relative to chordlength
     */
    void setSize(int, double, bool);
};
}  // namespace Surface

#endif
