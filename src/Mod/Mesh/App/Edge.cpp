#include <sstream>


#include "Edge.h"
#include "Mesh.h"


using namespace Mesh;

Edge::Edge()  // NOLINT
    : Mesh(nullptr)
{
    for (int i = 0; i < 2; i++) {
        PIndex[i] = MeshCore::POINT_INDEX_MAX;
        NIndex[i] = MeshCore::FACET_INDEX_MAX;
    }
}

Edge::Edge(const Edge& e)  // NOLINT
    : MeshCore::MeshGeomEdge(e)
    , Index(e.Index)
    , Mesh(e.Mesh)
{
    for (int i = 0; i < 2; i++) {
        PIndex[i] = e.PIndex[i];
        NIndex[i] = e.NIndex[i];
    }
}

Edge::Edge(Edge&& e)  // NOLINT
    : MeshCore::MeshGeomEdge(e)
    , Index(e.Index)
    , Mesh(e.Mesh)
{
    for (int i = 0; i < 2; i++) {
        PIndex[i] = e.PIndex[i];
        NIndex[i] = e.NIndex[i];
    }
}

Edge::~Edge() = default;

Edge& Edge::operator=(const Edge& e)
{
    Edge c = e;
    *this = std::move(c);
    return *this;
}

Edge& Edge::operator=(Edge&& e)
{
    MeshCore::MeshGeomEdge::operator=(e);
    Mesh = e.Mesh;
    Index = e.Index;
    for (int i = 0; i < 2; i++) {
        PIndex[i] = e.PIndex[i];
        NIndex[i] = e.NIndex[i];
    }

    return *this;
}

void Edge::unbound()
{
    Index = -1;
    Mesh = nullptr;
}
