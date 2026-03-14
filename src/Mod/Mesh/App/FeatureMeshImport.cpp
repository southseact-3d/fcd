#include "FeatureMeshImport.h"


using namespace Mesh;
using namespace MeshCore;

PROPERTY_SOURCE(Mesh::Import, Mesh::Feature)


Mesh::Import::Import()
{
    ADD_PROPERTY(FileName, (""));
}

short Mesh::Import::mustExecute() const
{
    if (FileName.isTouched()) {
        return 1;
    }
    return 0;
}

App::DocumentObjectExecReturn* Mesh::Import::execute()
{
    std::unique_ptr<MeshObject> apcKernel(new MeshObject());
    apcKernel->load(FileName.getValue());
    Mesh.setValuePtr(apcKernel.release());

    return App::DocumentObject::StdReturn;
}
