#include <Base/Matrix.h>

#include "FeatureMeshTransform.h"


using Base::Matrix4D;
using namespace Mesh;
using namespace MeshCore;

PROPERTY_SOURCE(Mesh::Transform, Mesh::Feature)

Transform::Transform()
{
    ADD_PROPERTY(Source, (nullptr));
    ADD_PROPERTY(Position, (Matrix4D()));
}

App::DocumentObjectExecReturn* Transform::execute()
{
    /*
        Feature* pcFirst = freecad_cast<Feature*>(Source.getValue());
        if (!pcFirst || pcFirst->isError())
            return new App::DocumentObjectExecReturn("Unknown Error");

        Matrix4D Matrix = Position.getValue();


        MeshCore::MeshKernel* pcKernel =
            new MeshCore::MeshKernel(pcFirst->Mesh.getValue());// Result Meshkernel
        pcKernel->Transform(Matrix);
        Mesh.setValue(pcKernel);
    */
    return App::DocumentObject::StdReturn;
}
