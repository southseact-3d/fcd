#ifndef REEN_REGIONGROWING_H
#define REEN_REGIONGROWING_H

#include <list>
#include <vector>

#include <Base/Vector3D.h>


namespace Points
{
class PointKernel;
}

namespace Reen
{

class RegionGrowing
{
public:
    RegionGrowing(const Points::PointKernel&, std::list<std::vector<int>>&);
    /** \brief Set the number of k nearest neighbors to use for the normal estimation.
     * \param[in] k the number of k-nearest neighbors
     */
    void perform(int ksearch);
    /** \brief Pass the normals to the points given in the constructor.
     * \param[in] normals the normals to the given points.
     */
    void perform(const std::vector<Base::Vector3f>& normals);

private:
    const Points::PointKernel& myPoints;
    std::list<std::vector<int>>& myClusters;
};

}  // namespace Reen

#endif  // REEN_REGIONGROWING_H
