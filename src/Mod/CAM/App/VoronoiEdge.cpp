#include "VoronoiEdge.h"


using namespace Base;
using namespace Path;

TYPESYSTEM_SOURCE(Path::VoronoiEdge, Base::Persistence)

VoronoiEdge::VoronoiEdge(Voronoi::diagram_type* d, long index)
    : dia(d)
    , index(index)
    , ptr(nullptr)
{
    if (dia && long(dia->num_edges()) > index) {
        ptr = &(dia->edges()[index]);
    }
}

VoronoiEdge::VoronoiEdge(Voronoi::diagram_type* d, const Voronoi::diagram_type::edge_type* e)
    : dia(d)
    , index(Voronoi::InvalidIndex)
    , ptr(e)
{
    if (d && e) {
        index = dia->index(e);
    }
}

VoronoiEdge::~VoronoiEdge()
{}

bool VoronoiEdge::isBound() const
{
    if (ptr && dia.isValid() && index != Voronoi::InvalidIndex) {
        if (&(dia->edges()[index]) == ptr) {
            return true;
        }
    }
    ptr = nullptr;
    return false;
}
