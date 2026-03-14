#include "FeatureMeshExport.h"
#include "MeshFeature.h"


using namespace Mesh;
using namespace MeshCore;

PROPERTY_SOURCE(Mesh::Export, App::DocumentObject)

Export::Export()
{
    ADD_PROPERTY(Source, (nullptr));
    ADD_PROPERTY(FileName, (""));
    ADD_PROPERTY(Format, (""));
}

short Export::mustExecute() const
{
    if (Source.getValue()) {
        if (Source.isTouched()) {
            return 1;
        }
        if (FileName.isTouched()) {
            return 1;
        }
        if (Format.isTouched()) {
            return 1;
        }
    }
    return 0;
}

App::DocumentObjectExecReturn* Export::execute()
{
    Mesh::Feature* pcFeat = dynamic_cast<Mesh::Feature*>(Source.getValue());
    if (!pcFeat || pcFeat->isError()) {
        return new App::DocumentObjectExecReturn("Cannot export invalid mesh feature");
    }

    pcFeat->Mesh.getValue().save(FileName.getValue());
    return App::DocumentObject::StdReturn;
}
