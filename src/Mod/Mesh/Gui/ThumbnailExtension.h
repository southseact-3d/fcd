#ifndef MESHGUI_THUMBNAIL_EXTENSION_H
#define MESHGUI_THUMBNAIL_EXTENSION_H

#include <Mod/Mesh/App/Exporter.h>
#include <Mod/Mesh/MeshGlobal.h>

namespace MeshGui
{

class ThumbnailExtension3MF: public Mesh::Extension3MF
{
public:
    Mesh::Extension3MF::Resource addMesh(const Mesh::MeshObject& mesh) override;

private:
    void setContentName(Mesh::Extension3MF::Resource&);

private:
    int index = 0;
};

class ThumbnailExtensionProducer: public Mesh::Extension3MFProducer
{
public:
    Mesh::AbstractFormatExtensionPtr create() const override
    {
        return std::make_shared<ThumbnailExtension3MF>();
    }
    void initialize() override
    {}
};

}  // namespace MeshGui


#endif  // MESHGUI_THUMBNAIL_EXTENSION_H
