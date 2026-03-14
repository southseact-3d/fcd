#ifndef MESH_IO_WRITER_IV_H
#define MESH_IO_WRITER_IV_H

#include <Mod/Mesh/App/Core/MeshIO.h>
#include <Mod/Mesh/MeshGlobal.h>

namespace MeshCore
{

/** Saves the mesh object into OpenInventor v2.1 format. */
class MeshExport WriterInventor
{
public:
    /*!
     * \brief WriterInventor
     */
    explicit WriterInventor(const MeshKernel& kernel, const Material*);
    /*!
     * \brief Apply a transformation for the exported mesh.
     */
    void SetTransform(const Base::Matrix4D&);
    /*!
     * \brief Save the mesh to an OpenInventor file.
     * \return true if the data could be written successfully, false otherwise.
     */
    bool Save(std::ostream&);

private:
    const MeshKernel& _kernel;
    const Material* _material;
    Base::Matrix4D _transform;
    bool apply_transform {false};
};

}  // namespace MeshCore


#endif  // MESH_IO_WRITER_IV_H
