#ifndef PATH_VORONOICELL_H
#define PATH_VORONOICELL_H

#include <Base/BaseClass.h>
#include "Voronoi.h"

namespace Path
{

class Voronoi;

class PathExport VoronoiCell: public Base::BaseClass
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    VoronoiCell(Voronoi::diagram_type* dia = nullptr, long index = Voronoi::InvalidIndex);
    VoronoiCell(Voronoi::diagram_type* dia, const Voronoi::diagram_type::cell_type* cell);
    ~VoronoiCell() override;

    bool isBound() const;

    Voronoi::point_type sourcePoint() const;
    Voronoi::segment_type sourceSegment() const;

    Base::Reference<Voronoi::diagram_type> dia;
    long index;
    mutable const Voronoi::diagram_type::cell_type* ptr;
};

}  // namespace Path
#endif
