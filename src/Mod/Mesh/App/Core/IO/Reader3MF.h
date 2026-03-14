#ifndef MESH_IO_READER_3MF_H
#define MESH_IO_READER_3MF_H

#include <Mod/Mesh/App/Core/MeshKernel.h>
#include <Mod/Mesh/MeshGlobal.h>
#include <iosfwd>
#include <memory>
#include <optional>
#include <unordered_map>
#include <xercesc/util/XercesDefs.hpp>

namespace XERCES_CPP_NAMESPACE
{
class DOMDocument;
class DOMElement;
class DOMNode;
class DOMNodeList;
class DOMNamedNodeMap;
class XercesDOMParser;
}  // namespace XERCES_CPP_NAMESPACE

namespace zipios
{
class FileCollection;
}

namespace MeshCore
{

/** Loads the mesh object from data in 3MF format. */
class MeshExport Reader3MF
{
public:
    /*!
     * \brief Reader3MF
     * \param str
     *
     * Passes an input stream to the constructor.
     */
    explicit Reader3MF(std::istream& str);

    /*!
     * \brief Reader3MF
     * \param filename
     *
     * Passes a file name to the constructor
     */
    explicit Reader3MF(const std::string& filename);
    /*!
     * \brief Load the mesh from the input stream or file
     * \return true on success and false otherwise
     */
    bool Load();
    std::vector<int> GetMeshIds() const;
    const MeshKernel& GetMesh(int id) const
    {
        return meshes.at(id).first;
    }
    const Base::Matrix4D& GetTransform(int id) const
    {
        return meshes.at(id).second;
    }

private:
    struct Component
    {
        int id = -1;
        int objectId = -1;
        std::string path;
        Base::Matrix4D transform;
    };
    static std::unique_ptr<XERCES_CPP_NAMESPACE::XercesDOMParser> makeDomParser();
    bool TryLoad();
    bool LoadModel(std::istream&);
    bool LoadModel(std::istream&, const Component&);
    bool TryLoadModel(std::istream&, const Component&);
    bool LoadModel(XERCES_CPP_NAMESPACE::DOMDocument&, const Component&);
    bool LoadResourcesAndBuild(XERCES_CPP_NAMESPACE::DOMElement*, const Component&);
    bool LoadResources(XERCES_CPP_NAMESPACE::DOMNodeList*, const Component&);
    bool LoadBuild(XERCES_CPP_NAMESPACE::DOMNodeList*);
    bool LoadItems(XERCES_CPP_NAMESPACE::DOMNodeList*);
    void LoadItem(XERCES_CPP_NAMESPACE::DOMNamedNodeMap*);
    bool LoadObject(XERCES_CPP_NAMESPACE::DOMNodeList*, const Component&);
    void LoadComponents(XERCES_CPP_NAMESPACE::DOMNodeList*, int id);
    void LoadComponent(XERCES_CPP_NAMESPACE::DOMNodeList*, int id);
    void LoadComponent(XERCES_CPP_NAMESPACE::DOMNamedNodeMap*, int id);
    void LoadMesh(XERCES_CPP_NAMESPACE::DOMNodeList*, int id, const Component&);
    void LoadVertices(XERCES_CPP_NAMESPACE::DOMNodeList*, MeshPointArray&);
    void ReadVertices(XERCES_CPP_NAMESPACE::DOMNodeList*, MeshPointArray&);
    void LoadTriangles(XERCES_CPP_NAMESPACE::DOMNodeList*, MeshFacetArray&);
    void ReadTriangles(XERCES_CPP_NAMESPACE::DOMNodeList*, MeshFacetArray&);
    bool LoadMeshFromComponents();
    std::optional<Base::Matrix4D> ReadTransform(XERCES_CPP_NAMESPACE::DOMNode*);

private:
    std::vector<Component> components;
    using MeshKernelAndTransform = std::pair<MeshKernel, Base::Matrix4D>;
    std::unordered_map<int, MeshKernelAndTransform> meshes;
    std::unique_ptr<zipios::FileCollection> file;
    std::unique_ptr<std::istream> zip;
};

}  // namespace MeshCore


#endif  // MESH_IO_READER_3MF_H
