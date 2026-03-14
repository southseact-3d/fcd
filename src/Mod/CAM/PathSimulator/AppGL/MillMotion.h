#ifndef __mill_operation_h__
#define __mill_operation_h__

#include "EndMill.h"
#include "linmath.h"
namespace MillSim
{

enum eEndMillType
{
    eEndmillFlat,
    eEndmillV,
    eEndmillBall,
    eEndmillFillet
};

enum eCmdType
{
    eNop,
    eMoveLiner,
    eRotateCW,
    eRotateCCW,
    eDril,
    eChangeTool
};

struct MillMotion
{
    eCmdType cmd;
    int tool;
    float x, y, z;
    float i, j, k;
    float r;
    char retract_mode = 0;
    float retract_z = NAN;
};

static inline void MotionPosToVec(vec3 vec, const MillMotion* motion)
{
    vec[0] = motion->x;
    vec[1] = motion->y;
    vec[2] = motion->z;
}
}  // namespace MillSim
#endif
