//this file originally part of Drawing workbench
//migrated to TechDraw workbench 2022-01-26 by Wandererfan

#ifndef TechDrawProjectionAlgos_h_
#define TechDrawProjectionAlgos_h_

#include <map>
#include <string>
#include <TopoDS_Shape.hxx>

#include <Base/Vector3D.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


class BRepAdaptor_Curve;

namespace TechDraw
{

  const TopoDS_Shape& build3dCurves(const TopoDS_Shape& shape);
  
/** Algo class for projecting shapes and creating SVG output of it
 */
class TechDrawExport ProjectionAlgos
{
public:
    /// Constructor
    ProjectionAlgos(const TopoDS_Shape &Input, const Base::Vector3d &Dir);
    virtual ~ProjectionAlgos();

    void execute();

    enum ExtractionType {
        Plain = 0,
        WithHidden = 1,
        WithSmooth = 2
    };

    using XmlAttributes = std::map<std::string,std::string>;

    std::string getSVG(ExtractionType type, double tolerance=0.05,
                       XmlAttributes V_style=XmlAttributes(),
                       XmlAttributes V0_style=XmlAttributes(),
                       XmlAttributes V1_style=XmlAttributes(),
                       XmlAttributes H_style=XmlAttributes(),
                       XmlAttributes H0_style=XmlAttributes(),
                       XmlAttributes H1_style=XmlAttributes());
    std::string getDXF(ExtractionType type, double scale, double tolerance);//added by Dan Falck 2011/09/25


    const TopoDS_Shape &Input;
    const Base::Vector3d &Direction;

    TopoDS_Shape V ;// hard edge visibly
    TopoDS_Shape V1;// Smoth edges visibly
    TopoDS_Shape VN;// contour edges visibly
    TopoDS_Shape VO;// contours apparents visibly
    TopoDS_Shape VI;// isoparamtriques   visibly
    TopoDS_Shape H ;// hard edge       invisibly
    TopoDS_Shape H1;// Smoth edges  invisibly
    TopoDS_Shape HN;// contour edges invisibly
    TopoDS_Shape HO;// contours apparents invisibly
    TopoDS_Shape HI;// isoparamtriques   invisibly
};

} //namespace TechDraw


#endif
