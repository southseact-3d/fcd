#include <Precision.hxx>
#include <Standard_Real.hxx>

#include "Blending/BlendPoint.h"
#include "Blending/BlendPointPy.h"


using namespace Surface;

BlendPoint::BlendPoint(const std::vector<Base::Vector3d>& vectorList)
    : vectors {vectorList}
{}

BlendPoint::BlendPoint()
{
    vectors.emplace_back(Base::Vector3d(0, 0, 0));
}

void BlendPoint::multiply(double f)
{
    for (int i = 0; i < nbVectors(); i++) {
        vectors[i] *= Pow(f, i);
    }
}

void BlendPoint::setSize(double f)
{
    if (nbVectors() > 1) {
        double il = vectors[1].Length();
        if (il > Precision::Confusion()) {
            multiply(f / il);
        }
    }
}

int BlendPoint::getContinuity()
{
    return vectors.size() - 1;
}

int BlendPoint::nbVectors()
{
    return vectors.size();
}
