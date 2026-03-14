#ifndef SKETCHER_ANALYSE_H
#define SKETCHER_ANALYSE_H

#include <vector>

#include "Constraint.h"


namespace Sketcher
{

struct ConstraintIds
{
    Base::Vector3d v;
    int First;
    int Second;
    Sketcher::PointPos FirstPos;
    Sketcher::PointPos SecondPos;
    Sketcher::ConstraintType Type;
};

struct Constraint_Equal
{
    using argument_type = ConstraintIds;
    using result_type = bool;
    struct Sketcher::ConstraintIds c;
    explicit Constraint_Equal(const ConstraintIds& c)
        : c(c)
    {}
    bool operator()(const ConstraintIds& x) const
    {
        if (c.First == x.First && c.FirstPos == x.FirstPos && c.Second == x.Second
            && c.SecondPos == x.SecondPos) {
            return true;
        }
        if (c.Second == x.First && c.SecondPos == x.FirstPos && c.First == x.Second
            && c.FirstPos == x.SecondPos) {
            return true;
        }
        return false;
    }
};

}  // namespace Sketcher

#endif  // SKETCHER_ANALYSE_H
