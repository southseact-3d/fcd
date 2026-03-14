#ifndef REEN_BSPLINEFITTING_H
#define REEN_BSPLINEFITTING_H

#if defined(HAVE_PCL_OPENNURBS)
# include <vector>

# include <Base/Vector3D.h>
# include <Geom_BSplineSurface.hxx>


namespace Reen
{

class BSplineFitting
{
public:
    BSplineFitting(const std::vector<Base::Vector3f>&);
    Handle(Geom_BSplineSurface) perform();

    void setIterations(unsigned);
    void setOrder(unsigned);
    void setRefinement(unsigned);
    void setInteriorSmoothness(double);
    void setInteriorWeight(double);
    void setBoundarySmoothness(double);
    void setBoundaryWeight(double);

private:
    std::vector<Base::Vector3f> myPoints;
    unsigned myIterations;
    unsigned myOrder;
    unsigned myRefinement;
    double myInteriorSmoothness;
    double myInteriorWeight;
    double myBoundarySmoothness;
    double myBoundaryWeight;
};

}  // namespace Reen

#endif  // HAVE_PCL_OPENNURBS

#endif  // REEN_BSPLINEFITTING_H
