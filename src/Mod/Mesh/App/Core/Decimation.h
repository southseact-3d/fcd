#ifndef MESH_DECIMATION_H
#define MESH_DECIMATION_H

#include <Mod/Mesh/MeshGlobal.h>

namespace MeshCore
{
class MeshKernel;

class MeshExport MeshSimplify
{
public:
    explicit MeshSimplify(MeshKernel&);
    void simplify(float tolerance, float reduction);
    void simplify(int targetSize);

private:
    MeshKernel& myKernel;
};

}  // namespace MeshCore


#endif  // MESH_DECIMATION_H
