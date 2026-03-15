// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_APP_BREPTESSELLATOR_H
#define COLOUR_APP_BREPTESSELLATOR_H

#include "UVAtlas.h"

class TopoDS_Shape;
class TopoDS_Face;

namespace Colour
{

/**
 * @brief Converts a B-Rep shape to a triangle mesh suitable for UV atlas generation
 * 
 * This class tessellates FreeCAD B-Rep shapes into triangle meshes that can be
 * used with the UV atlas generator. It uses the standard BRep_Tool triangulation
 * that FreeCAD uses for display.
 */
class BRepTessellator
{
public:
    struct TessellationOptions
    {
        /// Angular deflection for triangulation (lower = finer mesh)
        double angularDeflection {0.5};
        /// Linear deflection (maximum deviation from surface)
        double linearDeflection {0.01};
        /// Relative tolerance
        bool relativeDeflection {true};
    };

    BRepTessellator();
    ~BRepTessellator();

    /**
     * @brief Tessellate a B-Rep shape into vertices and triangles
     * 
     * @param shape The B-Rep shape to tessellate
     * @param outVertices Output vector for vertex data
     * @param outTriangles Output vector for triangle indices with face info
     * @param options Tessellation options
     * @return true if successful, false otherwise
     */
    bool tessellate(
        const TopoDS_Shape& shape,
        std::vector<Vertex>& outVertices,
        std::vector<Triangle>& outTriangles,
        const TessellationOptions& options = {}
    );

    /**
     * @brief Get the number of faces in the tessellated shape
     * @return Number of faces
     */
    int getFaceCount() const { return m_faceCount; }

    /**
     * @brief Set the tessellation quality
     * @param quality Quality value from 0.0 (coarse) to 1.0 (fine)
     */
    void setQuality(double quality);

    /**
     * @brief Get current tessellation options
     * @return Current options
     */
    TessellationOptions getOptions() const { return m_options; }

private:
    bool tessellateFace(
        const TopoDS_Face& face,
        int faceIndex,
        std::vector<Vertex>& outVertices,
        std::vector<Triangle>& outTriangles
    );

    TessellationOptions m_options;
    int m_faceCount {0};
};

} // namespace Colour

#endif // COLOUR_APP_BREPTESSELLATOR_H
