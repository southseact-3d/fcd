#ifndef MESH_MESHPOINT_H
#define MESH_MESHPOINT_H

#include <limits>

#include <Base/Vector3D.h>
#include <Base/Handle.h>

#ifndef MESH_GLOBAL_H
# include <Mod/Mesh/MeshGlobal.h>
#endif

using Base::Vector3d;

namespace Mesh
{
// forward declaration
class MeshObject;

/** The MeshPoint helper class
 * The MeshPoint class provides an interface for the MeshPointPy classes for
 * convenient access to the Mesh data structure. This class should not be used for
 * programming algorithms in C++. Use Mesh Core classes instead!
 */
class MeshExport MeshPoint: public Vector3d
{

public:
    /// simple constructor
    explicit MeshPoint(
        const Vector3d& vec = Vector3d(),
        const MeshObject* obj = nullptr,
        unsigned int index = std::numeric_limits<unsigned>::max()
    )
        : Vector3d(vec)
        , Index(index)
        , Mesh(obj)
    {}

    bool isBound() const
    {
        return Index != std::numeric_limits<unsigned>::max();
    }

    unsigned int Index;
    Base::Reference<const MeshObject> Mesh;
};

}  // namespace Mesh


#endif  // MESH_MESHPOINT_H
