#include "VoronoiVertex.h"


using namespace Base;
using namespace Path;

TYPESYSTEM_SOURCE(Path::VoronoiVertex, Base::Persistence)

VoronoiVertex::VoronoiVertex(Voronoi::diagram_type* d, long index)
    : dia(d)
    , index(index)
    , ptr(nullptr)
{
    if (dia && long(dia->num_vertices()) > index) {
        ptr = &(dia->vertices()[index]);
    }
}

VoronoiVertex::VoronoiVertex(Voronoi::diagram_type* d, const Voronoi::diagram_type::vertex_type* v)
    : dia(d)
    , index(Voronoi::InvalidIndex)
    , ptr(v)
{
    if (dia && v) {
        index = dia->index(v);
    }
}

VoronoiVertex::~VoronoiVertex()
{}

bool VoronoiVertex::isBound() const
{
    if (ptr && dia.isValid() && index != Voronoi::InvalidIndex) {
        if (&(dia->vertices()[index]) == ptr) {
            return true;
        }
    }
    ptr = nullptr;
    return false;
}
