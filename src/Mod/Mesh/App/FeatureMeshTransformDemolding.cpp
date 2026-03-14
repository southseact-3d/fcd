#include "FeatureMeshTransformDemolding.h"


using namespace Mesh;
using namespace MeshCore;

PROPERTY_SOURCE(Mesh::TransformDemolding, Mesh::Transform)


TransformDemolding::TransformDemolding()
{
    ADD_PROPERTY(Source, (nullptr));
    ADD_PROPERTY(Rotation, (0.0));
    ADD_PROPERTY(Axis, (0.0, 0.0, 1.0));
}

App::DocumentObjectExecReturn* TransformDemolding::execute()
{ /*
  Feature *pcFirst  = freecad_cast<Feature*>(Source.getValue());
  if (!pcFirst || pcFirst->isError())
      return new App::DocumentObjectExecReturn("Unknown Error");

  MeshCore::MeshKernel* pcKernel = new MeshCore::MeshKernel(pcFirst->Mesh.getValue()); // Result
  Meshkernel Base::Matrix4D trans(Base::Vector3f(0, 0, 0), Axis.getValue(), Rotation.getValue());
  //Matrix4D trans;
  //trans.rotLine(Axis.getValue(), Rotation.getValue());
  pcKernel->Transform(trans);
  Mesh.setValue(pcKernel);
 */
    return App::DocumentObject::StdReturn;
}
