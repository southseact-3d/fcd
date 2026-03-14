#include "EndMill.h"
#include "OpenGlWrapper.h"
#include "SimShapes.h"

using namespace MillSim;

EndMill::EndMill(int toolid, float diameter)
{
    radius = diameter / 2;
    toolId = toolid;
}

EndMill::EndMill(const std::vector<float>& toolProfile, int toolid, float diameter)
    : EndMill(toolid, diameter)
{
    profilePoints.clear();

    int srcBuffSize = toolProfile.size();
    nPoints = srcBuffSize / 2;
    if (nPoints < 2) {
        return;
    }

    // make sure last point is at 0,0 else, add it
    bool missingCenterPoint = fabs(toolProfile[nPoints * 2 - 2]) > 0.0001f;
    if (missingCenterPoint) {
        nPoints++;
    }

    int buffSize = PROFILE_BUFFER_SIZE(nPoints);
    profilePoints.resize(buffSize);

    // copy profile points
    for (int i = 0; i < srcBuffSize; i += 2) {
        // add some width to reduce simulation artifacts
        profilePoints[i] = toolProfile[i] + diameter * 0.01f;
        profilePoints[i + 1] = toolProfile[i + 1] - diameter * 0.01f;
    }
    if (missingCenterPoint) {
        profilePoints[srcBuffSize] = 0.0F;
        profilePoints[srcBuffSize + 1] = profilePoints[srcBuffSize - 1];
    }

    MirrorPointBuffer();
}

EndMill::~EndMill()
{
    toolShape.FreeResources();
    halfToolShape.FreeResources();
    pathShape.FreeResources();
}

void EndMill::GenerateDisplayLists(float quality)
{
    // calculate number of slices based on quality.
    int nslices = 16;
    if (quality < 3) {
        nslices = 4;
    }
    else if (quality < 7) {
        nslices = 8;
    }

    // full tool
    toolShape.RotateProfile(profilePoints.data(), nPoints, 0, 0, nslices, false);

    // half tool
    halfToolShape.RotateProfile(profilePoints.data(), nPoints, 0, 0, nslices / 2, true);

    // unit path
    int nFullPoints = PROFILE_BUFFER_POINTS(nPoints);
    pathShape.ExtrudeProfileLinear(profilePoints.data(), nFullPoints, 0, 1, 0, 0, true, false);
}

unsigned int EndMill::GenerateArcSegmentDL(float radius, float angleRad, float zShift, Shape* retShape)
{
    int nFullPoints = PROFILE_BUFFER_POINTS(nPoints);
    retShape->ExtrudeProfileRadial(profilePoints.data(), nFullPoints, radius, angleRad, zShift, true, true);
    return 0;
}

void EndMill::MirrorPointBuffer()
{
    int endpoint = PROFILE_BUFFER_POINTS(nPoints) - 1;
    for (int i = 0, j = endpoint * 2; i < (nPoints - 1) * 2; i += 2, j -= 2) {
        profilePoints[j] = -profilePoints[i];
        profilePoints[j + 1] = profilePoints[i + 1];
    }
}
