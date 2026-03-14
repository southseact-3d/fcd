#include "Axis.h"
#include "Placement.h"

using namespace Base;

Axis::Axis(const Vector3d& Orig, const Vector3d& Dir)
    : _base {Orig}
    , _dir {Dir}
{}

void Axis::reverse()
{
    this->_dir = -this->_dir;
}

Axis Axis::reversed() const
{
    Axis axis(*this);
    axis.reverse();
    return axis;
}

void Axis::move(const Vector3d& MovVec)
{
    _base += MovVec;
}

bool Axis::operator==(const Axis& that) const
{
    return (this->_base == that._base) && (this->_dir == that._dir);
}

bool Axis::operator!=(const Axis& that) const
{
    return !(*this == that);
}

Axis& Axis::operator*=(const Placement& plm)
{
    plm.multVec(this->_base, this->_base);
    plm.getRotation().multVec(this->_dir, this->_dir);
    return *this;
}

Axis Axis::operator*(const Placement& plm) const
{
    Axis axis(*this);
    axis *= plm;
    return axis;
}
