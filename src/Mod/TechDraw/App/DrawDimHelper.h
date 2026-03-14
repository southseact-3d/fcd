#ifndef DrawDimHelper_h_
#define DrawDimHelper_h_

#include <string>
#include <vector>

#include <gp_Pnt.hxx>
#include <TopoDS_Edge.hxx>

#include <Base/Vector3D.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DimensionReferences.h"


namespace TechDraw
{
class BaseGeom;
class DrawViewPart;
class DrawViewDimension;

/// Additional functions for working with Dimensions
class TechDrawExport DrawDimHelper {
    public:

    static DrawViewDimension* makeExtentDim(DrawViewPart* dvp,
                                            const std::string& dimType,
                                            ReferenceVector references2d);

    static DrawViewDimension* makeExtentDim(DrawViewPart* dvp,
                              std::vector<std::string> edgeNames,
                              int direction);

    static void makeExtentDim3d(DrawViewPart* dvp,
                                const std::string& dimType,
                                ReferenceVector references2d);
    static void makeExtentDim3d(DrawViewPart* dvp,
                                ReferenceVector references,
                                int direction);

    static gp_Pnt findClosestPoint(std::vector<TopoDS_Edge> inEdges,
                                   TopoDS_Edge& boundary);


    static DrawViewDimension* makeDistDim(DrawViewPart* dvp,
                                                    std::string dimType,
                                                    Base::Vector3d refMin,
                                                    Base::Vector3d refMax,
                                                    bool extent = false);

    static std::pair<Base::Vector3d, Base::Vector3d> minMax(DrawViewPart* dvp,
                                                            std::vector<std::string> edgeNames,
                                                            int direction);
    static std::pair<Base::Vector3d, Base::Vector3d> minMax3d(DrawViewPart* dvp,
                                                      ReferenceVector references,
                                                       int direction);
};

} //end namespace TechDraw
#endif
