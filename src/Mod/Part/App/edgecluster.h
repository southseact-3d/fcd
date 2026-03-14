#ifndef PART_EDGECLUSTER_H
#define PART_EDGECLUSTER_H

#include <map>
#include <vector>

#include <gp_Pnt.hxx>
#include <TopoDS_Edge.hxx>

#include <Mod/Part/PartGlobal.h>


namespace Part
{

struct Edgesort_gp_Pnt_Less
{
    bool operator()(const gp_Pnt& _Left, const gp_Pnt& _Right) const
    {
        Standard_Real x1, y1, z1, x2, y2, z2;
        _Left.Coord(x1, y1, z1);
        _Right.Coord(x2, y2, z2);
        if (fabs(x1 - x2) > 0.2) {
            return x1 < x2;
        }
        else if (fabs(y1 - y2) > 0.2) {
            return y1 < y2;
        }
        else if (fabs(z1 - z2) > 0.2) {
            return z1 < z2;
        }
        return false;
    }
};


using tEdgeVector = std::vector<TopoDS_Edge>;
using tMapPntEdge = std::map<gp_Pnt, tEdgeVector, Edgesort_gp_Pnt_Less>;
using tMapPntEdgePair = std::pair<gp_Pnt, tEdgeVector>;
using tEdgeClusterVector = std::vector<std::vector<TopoDS_Edge>>;


class PartExport Edgecluster
{
public:
    explicit Edgecluster(const std::vector<TopoDS_Edge>& usorted_edges);
    virtual ~Edgecluster();

    tEdgeClusterVector GetClusters();

private:
    void Perform();
    void Perform(const TopoDS_Edge& edge);
    bool PerformEdges(gp_Pnt& point);
    bool IsValidEdge(const TopoDS_Edge& edge);

    tEdgeClusterVector m_final_cluster;
    tEdgeVector m_unsortededges;
    tEdgeVector m_edges;

    tMapPntEdge m_vertices;
    bool m_done {false};

    tEdgeVector::const_iterator m_edgeIter;
};

}  // namespace Part

#endif  // PART_EDGECLUSTER_H
