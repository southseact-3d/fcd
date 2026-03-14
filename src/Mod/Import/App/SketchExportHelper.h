#include <Mod/Import/ImportGlobal.h>

#include <TopoDS_Shape.hxx>
#include <gp_Ax2.hxx>

namespace App
{
class DocumentObject;
}

namespace Import
{

/**
 * A class to assist with exporting sketches to dxf.
 */
class ImportExport SketchExportHelper
{
public:
    static TopoDS_Shape projectShape(const TopoDS_Shape& inShape, const gp_Ax2& projectionCS);
    static bool isSketch(App::DocumentObject* obj);
    static TopoDS_Shape getFlatSketchXY(App::DocumentObject* obj);
};

}  // namespace Import
