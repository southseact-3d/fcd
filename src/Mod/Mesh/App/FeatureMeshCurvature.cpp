#include "Core/Curvature.h"

#include "FeatureMeshCurvature.h"
#include "MeshFeature.h"


using namespace Mesh;

PROPERTY_SOURCE(Mesh::Curvature, App::DocumentObject)


Curvature::Curvature()
{
    ADD_PROPERTY(Source, (nullptr));
    ADD_PROPERTY(CurvInfo, (CurvatureInfo()));
}

short Curvature::mustExecute() const
{
    if (Source.isTouched()) {
        return 1;
    }
    if (Source.getValue() && Source.getValue()->isTouched()) {
        return 1;
    }
    return 0;
}

App::DocumentObjectExecReturn* Curvature::execute()
{
    Mesh::Feature* pcFeat = dynamic_cast<Mesh::Feature*>(Source.getValue());
    if (!pcFeat || pcFeat->isError()) {
        return new App::DocumentObjectExecReturn("No mesh object attached.");
    }

    // get all points
    const MeshCore::MeshKernel& rMesh = pcFeat->Mesh.getValue().getKernel();
    MeshCore::MeshCurvature meshCurv(rMesh);
    meshCurv.ComputePerVertex();
    const std::vector<MeshCore::CurvatureInfo>& curv = meshCurv.GetCurvature();

    std::vector<CurvatureInfo> values;
    values.reserve(curv.size());
    for (const auto& it : curv) {
        CurvatureInfo ci;
        ci.cMaxCurvDir = it.cMaxCurvDir;
        ci.cMinCurvDir = it.cMinCurvDir;
        ci.fMaxCurvature = it.fMaxCurvature;
        ci.fMinCurvature = it.fMinCurvature;
        values.push_back(ci);
    }

    CurvInfo.setValues(values);

    return App::DocumentObject::StdReturn;
}
