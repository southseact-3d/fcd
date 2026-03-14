#ifndef SURFACE_BLEND_POINT_H
#define SURFACE_BLEND_POINT_H


#include <Base/Vector3D.h>
#include <Mod/Surface/SurfaceGlobal.h>
#include <vector>


namespace Surface
{

/*!
 *   Create a list of vectors formed by a point and some derivatives
 *   obtained from a curve or surface
 */
class SurfaceExport BlendPoint
{
public:
    std::vector<Base::Vector3d> vectors;

    BlendPoint();
    /*!
     *  Constructor
     *\param std::vector<Base::Vector3d>
     */
    explicit BlendPoint(const std::vector<Base::Vector3d>& vectorList);
    ~BlendPoint() = default;
    /*!
     *  Scale the blendpoint vectors
     *\param double scaling factor
     */
    void multiply(double f);
    /*!
     * Resize the blendpoint vectors
     * by setting the size of the first derivative
     *\param double new size
     */
    void setSize(double f);
    /*!
     *\return continuity of this BlendPoint
     */
    int getContinuity();
    /*!
     *\return Number of vectors of this BlendPoint
     */
    int nbVectors();

private:
};
}  // namespace Surface

#endif
