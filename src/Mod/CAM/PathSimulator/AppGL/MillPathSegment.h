#ifndef __mill_path_segment_h__
#define __mill_path_segment_h__


#include "MillMotion.h"
#include "EndMill.h"
#include "linmath.h"
#include "MillPathLine.h"

namespace MillSim
{

enum MotionType
{
    MTVertical = 0,
    MTHorizontal,
    MTCurved
};

bool IsVerticalMotion(MillMotion* m1, MillMotion* m2);


class MillPathSegment
{
public:
    /// <summary>
    /// Create a mill path segment primitive
    /// </summary>
    /// <param name="endmill">Mill object</param>
    /// <param name="from">Start point</param>
    /// <param name="to">End point</param>
    MillPathSegment(EndMill* endmill, MillMotion* from, MillMotion* to);
    virtual ~MillPathSegment();


    virtual void AppendPathPoints(std::vector<MillPathPosition>& pointsBuffer);
    virtual void render(int substep);
    virtual void GetHeadPosition(vec3 headPos);
    static float SetQuality(float quality, float maxStockDimension);  // 1 minimum, 10 maximum

public:
    EndMill* endmill = nullptr;
    bool isMultyPart;
    int numSimSteps;
    int indexInArray;
    int segmentIndex;


protected:
    mat4x4 mShearMat;
    Shape mShape;
    float mXYDistance;
    float mXYZDistance;
    float mZDistance;
    float mXYAngle;
    float mStartAngRad;
    float mStepAngRad;
    float mStepDistance = 0;
    float mSweepAng;
    float mRadius = 0;
    float mArcDir = 0;
    bool mSmallRad = false;
    int mStepNumber = 0;

    static float mSmallRadStep;
    static float mResolution;

    vec3 mDiff;
    vec3 mStepLength = {0};
    vec3 mCenter = {0};
    vec3 mStartPos;
    vec3 mHeadPos = {0};
    MotionType mMotionType;
};
}  // namespace MillSim

#endif
