#ifndef MESHCORE_CURVATURE_H
#define MESHCORE_CURVATURE_H

#include "Definitions.h"
#include <Base/Vector3D.h>
#include <vector>

namespace MeshCore
{

class MeshKernel;
class MeshRefPointToFacets;

/** Curvature information. */
struct MeshExport CurvatureInfo
{
    float fMaxCurvature, fMinCurvature;
    Base::Vector3f cMaxCurvDir, cMinCurvDir;
};

class MeshExport FacetCurvature
{
public:
    FacetCurvature(const MeshKernel& kernel, const MeshRefPointToFacets& search, float, unsigned long);
    CurvatureInfo Compute(FacetIndex index) const;

private:
    const MeshKernel& myKernel;
    const MeshRefPointToFacets& mySearch;
    unsigned long myMinPoints;
    float myRadius;
};

class MeshExport MeshCurvature
{
public:
    explicit MeshCurvature(const MeshKernel& kernel);
    MeshCurvature(const MeshKernel& kernel, std::vector<FacetIndex> segm);
    float GetRadius() const
    {
        return myRadius;
    }
    void SetRadius(float r)
    {
        myRadius = r;
    }
    void ComputePerFace(bool parallel);
    void ComputePerVertex();
    const std::vector<CurvatureInfo>& GetCurvature() const
    {
        return myCurvature;
    }

private:
    const MeshKernel& myKernel;
    unsigned long myMinPoints;
    float myRadius;
    std::vector<FacetIndex> mySegment;
    std::vector<CurvatureInfo> myCurvature;
};

}  // namespace MeshCore

#endif  // MESHCORE_CURVATURE_H
