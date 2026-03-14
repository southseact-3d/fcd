#ifndef REEN_SEGMENTATION_H
#define REEN_SEGMENTATION_H

#include <list>
#include <vector>

#include <Base/Vector3D.h>


namespace Points
{
class PointKernel;
}

namespace Reen
{

class Segmentation
{
public:
    Segmentation(const Points::PointKernel&, std::list<std::vector<int>>& clusters);
    /** \brief Set the number of k nearest neighbors to use for the normal estimation.
     * \param[in] k the number of k-nearest neighbors
     */
    void perform(int ksearch);

private:
    const Points::PointKernel& myPoints;
    std::list<std::vector<int>>& myClusters;
};

class NormalEstimation
{
public:
    explicit NormalEstimation(const Points::PointKernel&);
    /** \brief Set the number of k nearest neighbors to use for the feature estimation.
     * \param[in] k the number of k-nearest neighbors
     */
    inline void setKSearch(int k)
    {
        kSearch = k;
    }

    /** \brief Set the sphere radius that is to be used for determining the nearest neighbors used
     * for the feature estimation. \param[in] radius the sphere radius used as the maximum distance
     * to consider a point a neighbor
     */
    inline void setSearchRadius(double radius)
    {
        searchRadius = radius;
    }

    /** \brief Perform the normal estimation.
     * \param[out] the estimated normals
     */
    void perform(std::vector<Base::Vector3d>& normals);

private:
    const Points::PointKernel& myPoints;
    int kSearch;
    double searchRadius;
};

}  // namespace Reen

#endif  // REEN_SEGMENTATION_H
