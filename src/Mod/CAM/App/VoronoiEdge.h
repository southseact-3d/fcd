#ifndef PATH_VORONOIEDGE_H
#define PATH_VORONOIEDGE_H

#include <Base/BaseClass.h>
#include "Voronoi.h"


namespace Path
{

class Voronoi;

class PathExport VoronoiEdge: public Base::BaseClass
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    VoronoiEdge(Voronoi::diagram_type* dia = nullptr, long index = Voronoi::InvalidIndex);
    VoronoiEdge(Voronoi::diagram_type* dia, const Voronoi::diagram_type::edge_type* edge);
    ~VoronoiEdge() override;

    bool isBound() const;

    Base::Reference<Voronoi::diagram_type> dia;
    long index;
    mutable const Voronoi::diagram_type::edge_type* ptr;
};

}  // namespace Path
#endif
