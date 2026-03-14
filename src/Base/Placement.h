#ifndef BASE_PLACEMENT_H
#define BASE_PLACEMENT_H

#include <string>

#include "Rotation.h"


namespace Base
{

class DualQuat;
class Matrix4D;

template<typename T>
class Vector3;
using Vector3d = Vector3<double>;

/**
 * The Placement class.
 */
class BaseExport Placement
{
public:
    /// default constructor
    Placement();
    Placement(const Placement&) = default;
    Placement(Placement&&) = default;
    Placement(const Base::Matrix4D& matrix);
    Placement(const Vector3d& Pos, const Rotation& Rot);
    Placement(const Vector3d& Pos, const Rotation& Rot, const Vector3d& Cnt);

    /** specialty constructors */
    //@{
    static Placement fromDualQuaternion(DualQuat qq);
    //@}

    /// Destruction
    ~Placement() = default;

    Matrix4D toMatrix() const;
    void fromMatrix(const Matrix4D& m);
    DualQuat toDualQuaternion() const;
    const Vector3d& getPosition() const
    {
        return _pos;
    }
    const Rotation& getRotation() const
    {
        return _rot;
    }
    void setPosition(const Vector3d& Pos)
    {
        _pos = Pos;
    }
    void setRotation(const Rotation& Rot)
    {
        _rot = Rot;
    }

    bool isIdentity() const;
    bool isIdentity(double tol) const;
    void invert();
    Placement inverse() const;
    void move(const Vector3d& MovVec);

    bool isSame(const Placement&) const;
    bool isSame(const Placement&, double tol) const;

    /** Operators. */
    //@{
    Placement& operator*=(const Placement& p);
    Placement operator*(const Placement& p) const;
    bool operator==(const Placement&) const;
    bool operator!=(const Placement&) const;
    Placement& operator=(const Placement&) = default;
    Placement& operator=(Placement&&) = default;
    Placement pow(double t, bool shorten = true) const;

    Placement& multRight(const Base::Placement& p);
    Placement& multLeft(const Base::Placement& p);

    void multVec(const Vector3d& src, Vector3d& dst) const;
    void multVec(const Vector3f& src, Vector3f& dst) const;
    //@}

    static Placement slerp(const Placement& p0, const Placement& p1, double t);
    static Placement sclerp(const Placement& p0, const Placement& p1, double t, bool shorten = true);

    /// Returns string representation of the placement, useful for debugging
    std::string toString() const;

private:
    Vector3<double> _pos;
    Base::Rotation _rot;
};

}  // namespace Base


#endif  // BASE_PLACEMENT_H
