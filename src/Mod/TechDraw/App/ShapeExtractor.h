#ifndef ShapeExtractor_h_
#define ShapeExtractor_h_

#include <TopoDS_Shape.hxx>

#include <App/DocumentObject.h>
#include <App/Link.h>
#include <Base/Type.h>
#include <Base/Vector3D.h>

#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDraw
{

class TechDrawExport ShapeExtractor
{
public:
    static TopoDS_Shape getShapes(const std::vector<App::DocumentObject*> links, bool include2d = true);
    static std::vector<TopoDS_Shape> getShapes2d(const std::vector<App::DocumentObject*> links);
    static std::vector<TopoDS_Shape> getXShapes(const App::Link* xLink);
    static TopoDS_Shape getShapesFused(const std::vector<App::DocumentObject*> links);
    static TopoDS_Shape getShapeFromXLink(const App::Link* xLink);
    static std::vector<TopoDS_Shape> getShapesFromXRoot(const App::DocumentObject *xLinkRoot);
    static std::vector<TopoDS_Shape> getShapesFromObject(const App::DocumentObject* docObj);

    static bool is2dObject(const App::DocumentObject* obj);
    static bool isEdgeType(const App::DocumentObject* obj);
    static bool isPointType(const App::DocumentObject* obj);
    static bool isDraftPoint(const App::DocumentObject* obj);
    static bool isDatumPoint(const App::DocumentObject* obj);
    static bool isSketchObject(const App::DocumentObject* obj);
    static bool isExplodedAssembly(const App::DocumentObject* obj);

    static Base::Vector3d getLocation3dFromFeat(const App::DocumentObject *obj);
    static TopoDS_Shape getLocatedShape(const App::DocumentObject* docObj);

    static bool checkShape(const App::DocumentObject* shapeObj, TopoDS_Shape shape);

    static App::DocumentObject* getExplodedAssembly(std::vector<TopoDS_Shape>& sourceShapes,
                                                    App::DocumentObject* link);
    static void restoreExplodedAssembly(App::DocumentObject* link);

    static App::DocumentObject* getLinkedObject(const App::DocumentObject* root);

protected:

private:

};

} //namespace TechDraw

#endif  // #ifndef ShapeExtractor_h_
