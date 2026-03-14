#ifndef SURFACEGUI_SELECTIONMODE_H
#define SURFACEGUI_SELECTIONMODE_H

namespace SurfaceGui
{

enum SelectionMode
{
    None = 0,
    InitFace = 1,
    AppendEdge = 2,
    RemoveEdge = 3,
    AppendEdgeConstraint = 4,
    RemoveEdgeConstraint = 5,
    AppendVertexConstraint = 6,
    RemoveVertexConstraint = 7,
};

}  // namespace SurfaceGui

#endif  // SURFACEGUI_SELECTIONMODE_H
