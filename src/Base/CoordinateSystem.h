#ifndef BASE_COORDINATESYSTEM_H
#define BASE_COORDINATESYSTEM_H

#include "Axis.h"

namespace Base
{
class Rotation;

/**
 * Describes a right-handed coordinate system in 3D space.
 \author Werner Mayer
 */
class BaseExport CoordinateSystem
{
public:
    /** Construct a default coordinate system with position in (0,0,0),
     * with X axis (1,0,0), with Y axis (0,1,0) and Z axis (0,0,1)
     */
    CoordinateSystem();
    CoordinateSystem(const CoordinateSystem&) = default;
    CoordinateSystem(CoordinateSystem&&) = default;
    ~CoordinateSystem() = default;

    CoordinateSystem& operator=(const CoordinateSystem&) = default;
    CoordinateSystem& operator=(CoordinateSystem&&) = default;

    /** Sets the main axis. X and Y dir are adjusted accordingly.
     * The main axis \a v must not be parallel to the X axis
     */
    void setAxis(const Axis& axis);
    /** Sets the main axis. X and Y dir are adjusted accordingly.
     * The main axis must not be parallel to \a xd
     */
    void setAxes(const Axis&, const Vector3d& xd);
    /** Sets the main axis. X and Y dir are adjusted accordingly.
     * The main axis \a n must not be parallel to \a xd
     */
    void setAxes(const Vector3d& n, const Vector3d& xd);
    inline const Axis& getAxis() const
    {
        return axis;
    }

    /** The passed vector must not be parallel to the main axis */
    void setXDirection(const Vector3d&);
    inline const Vector3d& getXDirection() const
    {
        return xdir;
    }

    /** The passed vector must not be parallel to the main axis */
    void setYDirection(const Vector3d&);
    inline const Vector3d& getYDirection() const
    {
        return ydir;
    }

    /** Sets the main axis. X and Y dir are adjusted accordingly.
     * The main axis must not be parallel to the X axis
     */
    void setZDirection(const Vector3d&);
    inline const Vector3d& getZDirection() const
    {
        return axis.getDirection();
    }
    inline void setPosition(const Vector3d& pos)
    {
        axis.setBase(pos);
    }
    inline const Vector3d& getPosition() const
    {
        return axis.getBase();
    }

    /** This computes the displacement from this coordinate system to the
     * given coordinate system \a cs
     */
    Placement displacement(const CoordinateSystem& cs) const;

    /** Transform the point \a pnt to be in this coordinate system */
    void transformTo(Vector3d& pnt);

    /** Apply the placement \a plm to the coordinate system. */
    void transform(const Placement& plm);

    /** Apply the rotation \a rot to the coordinate system. */
    void transform(const Rotation& rot);

    /** Set the placement \a plm to the coordinate system. */
    void setPlacement(const Placement& plm);

private:
    Axis axis;
    Vector3d xdir;
    Vector3d ydir;
};

}  // namespace Base

#endif  // BASE_COORDINATESYSTEM_H
