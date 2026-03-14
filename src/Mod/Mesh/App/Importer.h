#ifndef MESH_IMPORTER_H
#define MESH_IMPORTER_H

#include <string>
#include <vector>

namespace App
{
class Document;
class Color;
}  // namespace App

namespace MeshCore
{
struct Material;
}
namespace Mesh
{
class MeshObject;
class Feature;

class Importer
{
public:
    explicit Importer(App::Document*);
    void load(const std::string& fileName);

private:
    void addVertexColors(Feature*, const std::vector<Base::Color>&);
    void addFaceColors(Feature*, const std::vector<Base::Color>&);
    void addColors(Feature*, const std::string& property, const std::vector<Base::Color>&);
    Feature* createMesh(const std::string& name, MeshObject&);
    void createMeshFromSegments(const std::string& name, MeshCore::Material& mat, MeshObject& mesh);

private:
    App::Document* document;
};

}  // namespace Mesh

#endif  // MESH_IMPORTER_H
