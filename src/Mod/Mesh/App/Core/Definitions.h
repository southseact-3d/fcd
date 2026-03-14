#ifndef MESH_DEFINITIONS_H
#define MESH_DEFINITIONS_H

#ifndef MESH_GLOBAL_H
# include <Mod/Mesh/MeshGlobal.h>
#endif

#include <limits>

// default values
#define MESH_MIN_PT_DIST 1.0e-6F
#define MESH_MIN_EDGE_LEN 1.0e-3F
#define MESH_MIN_EDGE_ANGLE 2.0
#define MESH_REMOVE_MIN_LEN true
#define MESH_REMOVE_G3_EDGES true

namespace MeshCore
{

// type definitions
using ElementIndex = unsigned long;
const ElementIndex ELEMENT_INDEX_MAX = std::numeric_limits<unsigned long>::max();
using FacetIndex = ElementIndex;
const FacetIndex FACET_INDEX_MAX = std::numeric_limits<unsigned long>::max();
using PointIndex = ElementIndex;
const PointIndex POINT_INDEX_MAX = std::numeric_limits<unsigned long>::max();

template<class Prec>
class Math
{
public:
    MeshExport static const Prec PI;
};

using Mathf = Math<float>;
using Mathd = Math<double>;

/**
 * Global defined tolerances used to compare points
 * for equality.
 */
class MeshExport MeshDefinitions
{
public:
    MeshDefinitions();

    static float _fMinPointDistance;
    static float _fMinPointDistanceP2;
    static float _fMinPointDistanceD1;

    static float _fMinEdgeLength;
    static bool _bRemoveMinLength;

    static float _fMinEdgeAngle;

    static void SetMinPointDistance(float fMin);
};

}  // namespace MeshCore

#endif  // MESH_DEFINITIONS_H
