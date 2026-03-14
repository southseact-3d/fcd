#include "StockObject.h"
#include "Shader.h"

#define NUM_PROFILE_POINTS 4
using namespace MillSim;

StockObject::StockObject()
{}

void StockObject::GenerateBoxStock(float x, float y, float z, float l, float w, float h)
{
    int idx = 0;
    SET_DUAL(mProfile, idx, y + w, z + h);
    SET_DUAL(mProfile, idx, y + w, z);
    SET_DUAL(mProfile, idx, y, z);
    SET_DUAL(mProfile, idx, y, z + h);

    vec3_set(position, x, y, z);
    vec3_set(center, x + l / 2, y + w / 2, z + h / 2);
    vec3_set(size, l, w, h);

    shape.ExtrudeProfileLinear(mProfile, NUM_PROFILE_POINTS, x, x + l, 0, 0, true, true);
}
