#ifndef __end_mill_h__
#define __end_mill_h__

#include "SimShapes.h"
#include <vector>

#define PROFILE_BUFFER_POINTS(npoints) ((npoints) * 2 - 1)
#define PROFILE_BUFFER_SIZE(npoints) (PROFILE_BUFFER_POINTS(npoints) * 2)
#define MILL_HEIGHT 10

namespace MillSim
{
class EndMill
{
public:
    std::vector<float> profilePoints;
    float radius;
    int nPoints = 0;
    int toolId = -1;

    Shape pathShape;
    Shape halfToolShape;
    Shape toolShape;

public:
    EndMill(int toolid, float diameter);
    EndMill(const std::vector<float>& toolProfile, int toolid, float diameter);
    virtual ~EndMill();
    void GenerateDisplayLists(float quality);
    unsigned int GenerateArcSegmentDL(float radius, float angleRad, float zShift, Shape* retShape);

protected:
    void MirrorPointBuffer();
};
}  // namespace MillSim

#endif
