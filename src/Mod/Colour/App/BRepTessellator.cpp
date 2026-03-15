// SPDX-License-Identifier: LGPL-2.1-or-later

#include "BRepTessellator.h"

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

namespace Colour
{

BRepTessellator::BRepTessellator() = default;

BRepTessellator::~BRepTessellator() = default;

void BRepTessellator::setQuality(double quality)
{
    // Map quality (0.0-1.0) to deflection values
    // 0.0 = coarse (large deflection), 1.0 = fine (small deflection)
    quality = std::max(0.0, std::min(1.0, quality));
    
    // Linear deflection: 0.1 (coarse) to 0.001 (fine)
    m_options.linearDeflection = 0.1 - (quality * 0.099);
    
    // Angular deflection: 0.8 (coarse) to 0.1 (fine)
    m_options.angularDeflection = 0.8 - (quality * 0.7);
}

bool BRepTessellator::tessellate(
    const TopoDS_Shape& shape,
    std::vector<Vertex>& outVertices,
    std::vector<Triangle>& outTriangles,
    const TessellationOptions& options)
{
    outVertices.clear();
    outTriangles.clear();
    
    if (shape.IsNull()) {
        return false;
    }
    
    m_options = options;
    m_faceCount = 0;
    
    // First, compute the mesh using BRepMesh_IncrementalMesh
    BRepMesh_IncrementalMesh meshGen(shape, m_options.linearDeflection, 
                                     m_options.relativeDeflection, 
                                     m_options.angularDeflection, true);
    
    // Now iterate over faces and extract triangulation
    int faceIndex = 0;
    for (TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next(), ++faceIndex) {
        TopoDS_Face face = TopoDS::Face(exp.Current());
        
        if (!tessellateFace(face, faceIndex, outVertices, outTriangles)) {
            // Continue with other faces even if one fails
            continue;
        }
    }
    
    m_faceCount = faceIndex;
    return !outTriangles.empty();
}

bool BRepTessellator::tessellateFace(
    const TopoDS_Face& face,
    int faceIndex,
    std::vector<Vertex>& outVertices,
    std::vector<Triangle>& outTriangles)
{
    TopLoc_Location location;
    Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);
    
    if (triangulation.IsNull()) {
        return false;
    }
    
    // Get face normal for consistent orientation
    BRepAdaptor_Surface surf(face);
    gp_Vec faceNormal;
    bool hasNormal = false;
    
    // Get the surface normal at the center
    double u1, u2, v1, v2;
    BRepTools::UVBounds(face, u1, u2, v1, v2);
    double umid = (u1 + u2) / 2.0;
    double vmid = (v1 + v2) / 2.0;
    
    gp_Pnt pt;
    gp_Vec du, dv;
    try {
        surf.D1(umid, vmid, pt, du, dv);
        faceNormal = du.Crossed(dv);
        if (faceNormal.Magnitude() > 1.0e-7) {
            faceNormal.Normalize();
            hasNormal = true;
        }
    } catch (...) {
        // Surface evaluation failed, will use triangle normals
    }
    
    // Track vertex offset for this face
    std::size_t vertexOffset = outVertices.size();
    
    // Extract vertices
    const TColgp_Array1OfPnt& nodes = triangulation->Nodes();
    for (int i = nodes.Lower(); i <= nodes.Upper(); ++i) {
        gp_Pnt pnt = nodes(i).Transformed(location.Transformation());
        
        Vertex vertex;
        vertex.position.x = static_cast<float>(pnt.X());
        vertex.position.y = static_cast<float>(pnt.Y());
        vertex.position.z = static_cast<float>(pnt.Z());
        
        // Calculate vertex normal
        if (hasNormal) {
            vertex.normal.x = static_cast<float>(faceNormal.X());
            vertex.normal.y = static_cast<float>(faceNormal.Y());
            vertex.normal.z = static_cast<float>(faceNormal.Z());
        } else {
            // Will compute from triangles later
            vertex.normal = {0.0f, 0.0f, 1.0f};
        }
        
        outVertices.push_back(vertex);
    }
    
    // Extract triangles
    const Poly_Array1OfTriangle& triangles = triangulation->Triangles();
    for (int i = triangles.Lower(); i <= triangles.Upper(); ++i) {
        int i1, i2, i3;
        triangles(i).Get(i1, i2, i3);
        
        Triangle tri;
        tri.indices[0] = static_cast<std::uint32_t>(vertexOffset + i1 - 1);  // OCCT uses 1-based indexing
        tri.indices[1] = static_cast<std::uint32_t>(vertexOffset + i2 - 1);
        tri.indices[2] = static_cast<std::uint32_t>(vertexOffset + i3 - 1);
        tri.faceIndex = faceIndex;
        
        // Check orientation - reverse if face is reversed
        if (face.Orientation() == TopAbs_REVERSED) {
            std::swap(tri.indices[1], tri.indices[2]);
        }
        
        outTriangles.push_back(tri);
    }
    
    return true;
}

} // namespace Colour
