#ifndef __solid_object_h__
#define __solid_object_h__
#include "SimShapes.h"
#include "linmath.h"
#include <vector>

namespace MillSim
{

class SolidObject
{
public:
    SolidObject();
    virtual ~SolidObject();
    void SetPosition(vec3 position);

    /// Calls the display list.
    virtual void render();
    Shape shape;
    void GenerateSolid(const std::vector<Vertex>& verts, const std::vector<GLushort>& indices);
    vec3 center = {};
    vec3 size = {};
    vec3 position = {};
    bool isValid = false;

protected:
    mat4x4 mModelMat;
};
}  // namespace MillSim

#endif
