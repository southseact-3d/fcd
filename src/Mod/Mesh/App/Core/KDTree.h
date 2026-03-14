#ifndef MESH_KDTREE_H
#define MESH_KDTREE_H

#include "Elements.h"

namespace MeshCore
{

class MeshExport MeshKDTree
{
public:
    MeshKDTree();
    explicit MeshKDTree(const std::vector<Base::Vector3f>& points);
    explicit MeshKDTree(const MeshPointArray& points);
    ~MeshKDTree();

    void AddPoint(const Base::Vector3f& point);
    void AddPoints(const std::vector<Base::Vector3f>& points);
    void AddPoints(const MeshPointArray& points);

    bool IsEmpty() const;
    void Clear();
    void Optimize();

    PointIndex FindNearest(const Base::Vector3f& p, Base::Vector3f& n, float&) const;
    PointIndex FindNearest(const Base::Vector3f& p, float max_dist, Base::Vector3f& n, float&) const;
    PointIndex FindExact(const Base::Vector3f& p) const;
    void FindInRange(const Base::Vector3f&, float, std::vector<PointIndex>&) const;

    MeshKDTree(const MeshKDTree&) = delete;
    MeshKDTree(MeshKDTree&&) = delete;
    void operator=(const MeshKDTree&) = delete;
    void operator=(MeshKDTree&&) = delete;

private:
    class Private;
    Private* d;
};

}  // namespace MeshCore


#endif  // MESH_KDTREE_H
