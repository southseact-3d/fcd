#ifndef PATH_VORONOIVERTEX_H
#define PATH_VORONOIVERTEX_H

#include <Base/BaseClass.h>
#include "Voronoi.h"


namespace Path
{

class Voronoi;

class PathExport VoronoiVertex: public Base::BaseClass
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    VoronoiVertex(Voronoi::diagram_type* dia = nullptr, long index = Voronoi::InvalidIndex);
    VoronoiVertex(Voronoi::diagram_type* dia, const Voronoi::diagram_type::vertex_type* v);
    ~VoronoiVertex() override;

    bool isBound() const;

    Base::Reference<Voronoi::diagram_type> dia;
    long index;
    mutable const Voronoi::diagram_type::vertex_type* ptr;
};

}  // namespace Path
#endif
