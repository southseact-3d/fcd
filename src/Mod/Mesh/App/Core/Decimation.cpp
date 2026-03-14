#include "Decimation.h"
#include "MeshKernel.h"
#include "Simplify.h"


using namespace MeshCore;

MeshSimplify::MeshSimplify(MeshKernel& mesh)
    : myKernel(mesh)
{}

void MeshSimplify::simplify(float tolerance, float reduction)
{
    Simplify alg;

    const MeshPointArray& points = myKernel.GetPoints();
    for (std::size_t i = 0; i < points.size(); i++) {
        Simplify::Vertex v;
        v.tstart = 0;
        v.tcount = 0;
        v.border = 0;
        v.p = points[i];
        alg.vertices.push_back(v);
    }

    const MeshFacetArray& facets = myKernel.GetFacets();
    for (std::size_t i = 0; i < facets.size(); i++) {
        Simplify::Triangle t;
        t.deleted = 0;
        t.dirty = 0;
        for (double& j : t.err) {
            j = 0.0;
        }
        for (int j = 0; j < 3; j++) {
            t.v[j] = facets[i]._aulPoints[j];
        }
        alg.triangles.push_back(t);
    }

    int target_count = static_cast<int>(static_cast<float>(facets.size()) * (1.0F - reduction));

    // Simplification starts
    alg.simplify_mesh(target_count, tolerance);

    // Simplification done
    MeshPointArray new_points;
    new_points.reserve(alg.vertices.size());
    for (const auto& vertex : alg.vertices) {
        new_points.push_back(vertex.p);
    }

    std::size_t numFacets = 0;
    for (const auto& triangle : alg.triangles) {
        if (!triangle.deleted) {
            numFacets++;
        }
    }
    MeshFacetArray new_facets;
    new_facets.reserve(numFacets);
    for (const auto& triangle : alg.triangles) {
        if (!triangle.deleted) {
            MeshFacet face;
            face._aulPoints[0] = triangle.v[0];
            face._aulPoints[1] = triangle.v[1];
            face._aulPoints[2] = triangle.v[2];
            new_facets.push_back(face);
        }
    }

    myKernel.Adopt(new_points, new_facets, true);
}

void MeshSimplify::simplify(int targetSize)
{
    Simplify alg;

    const MeshPointArray& points = myKernel.GetPoints();
    for (std::size_t i = 0; i < points.size(); i++) {
        Simplify::Vertex v;
        v.tstart = 0;
        v.tcount = 0;
        v.border = 0;
        v.p = points[i];
        alg.vertices.push_back(v);
    }

    const MeshFacetArray& facets = myKernel.GetFacets();
    for (std::size_t i = 0; i < facets.size(); i++) {
        Simplify::Triangle t;
        t.deleted = 0;
        t.dirty = 0;
        for (double& j : t.err) {
            j = 0.0;
        }
        for (int j = 0; j < 3; j++) {
            t.v[j] = facets[i]._aulPoints[j];
        }
        alg.triangles.push_back(t);
    }

    // Simplification starts
    alg.simplify_mesh(targetSize, std::numeric_limits<float>::max());

    // Simplification done
    MeshPointArray new_points;
    new_points.reserve(alg.vertices.size());
    for (const auto& vertex : alg.vertices) {
        new_points.push_back(vertex.p);
    }

    std::size_t numFacets = 0;
    for (const auto& triangle : alg.triangles) {
        if (!triangle.deleted) {
            numFacets++;
        }
    }
    MeshFacetArray new_facets;
    new_facets.reserve(numFacets);
    for (const auto& triangle : alg.triangles) {
        if (!triangle.deleted) {
            MeshFacet face;
            face._aulPoints[0] = triangle.v[0];
            face._aulPoints[1] = triangle.v[1];
            face._aulPoints[2] = triangle.v[2];
            new_facets.push_back(face);
        }
    }

    myKernel.Adopt(new_points, new_facets, true);
}
