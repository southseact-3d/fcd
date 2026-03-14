#ifndef SKETCHERGUI_AutoConstraint_H
#define SKETCHERGUI_AutoConstraint_H

#include <Mod/Sketcher/App/Constraint.h>

namespace SketcherGui
{

// A Simple data type to hold basic information for suggested constraints
struct AutoConstraint
{
    enum TargetType
    {
        VERTEX,
        CURVE,
        VERTEX_NO_TANGENCY
    };
    Sketcher::ConstraintType Type;
    int GeoId;
    Sketcher::PointPos PosId;
};

}  // namespace SketcherGui


#endif  // SKETCHERGUI_AutoConstraint_H
