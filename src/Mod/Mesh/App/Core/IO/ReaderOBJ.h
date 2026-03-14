#ifndef MESH_IO_READER_OBJ_H
#define MESH_IO_READER_OBJ_H

#include <Mod/Mesh/App/Core/MeshKernel.h>
#include <Mod/Mesh/MeshGlobal.h>
#include <iosfwd>

namespace MeshCore
{

class MeshKernel;
struct Material;

/** Loads the mesh object from data in OBJ format. */
class MeshExport ReaderOBJ
{
public:
    /*!
     * \brief ReaderOBJ
     */
    explicit ReaderOBJ(MeshKernel& kernel, Material*);
    /*!
     * \brief Load the mesh from the file
     * \return true on success and false otherwise
     */
    bool Load(const std::string& file);
    /*!
     * \brief Load the mesh from the input stream
     * \return true on success and false otherwise
     */
    bool Load(std::istream& str);
    /*!
     * \brief Load the material file to the corresponding OBJ file.
     * This function must be called after \ref Load().
     * \param str
     * \return  true on success and false otherwise
     */
    bool LoadMaterial(std::istream& str);

    const std::vector<std::string>& GetGroupNames() const
    {
        return _groupNames;
    }

private:
    MeshKernel& _kernel;
    Material* _material;
    std::vector<std::string> _groupNames;
    std::vector<std::pair<std::string, unsigned long>> _materialNames;
};

}  // namespace MeshCore


#endif  // MESH_IO_READER_OBJ_H
