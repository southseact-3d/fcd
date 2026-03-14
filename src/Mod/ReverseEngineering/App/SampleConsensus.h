#ifndef REEN_SAMPLECONSENSUS_H
#define REEN_SAMPLECONSENSUS_H

#include <vector>

#include <Base/Vector3D.h>


namespace Points
{
class PointKernel;
}

namespace Reen
{

class SampleConsensus
{
public:
    enum SacModel
    {
        SACMODEL_PLANE,
        SACMODEL_LINE,
        SACMODEL_CIRCLE2D,
        SACMODEL_CIRCLE3D,
        SACMODEL_SPHERE,
        SACMODEL_CYLINDER,
        SACMODEL_CONE,
        SACMODEL_TORUS,
    };
    SampleConsensus(SacModel sac, const Points::PointKernel&, const std::vector<Base::Vector3d>&);
    double perform(std::vector<float>& parameters, std::vector<int>& model);

private:
    SacModel mySac;
    const Points::PointKernel& myPoints;
    const std::vector<Base::Vector3d>& myNormals;
};

}  // namespace Reen

#endif  // REEN_SAMPLECONSENSUS_H
