#include "FeatureGeometrySet.h"


using namespace Part;


PROPERTY_SOURCE(Part::FeatureGeometrySet, Part::Feature)


FeatureGeometrySet::FeatureGeometrySet()
{
    ADD_PROPERTY(GeometrySet, (nullptr));
}


App::DocumentObjectExecReturn* FeatureGeometrySet::execute()
{
    TopoShape result;

    const std::vector<Geometry*>& Geoms = GeometrySet.getValues();

    bool first = true;
    for (auto Geom : Geoms) {
        TopoDS_Shape sh = Geom->toShape();
        if (first) {
            first = false;
            result.setShape(sh);
        }
        else {
            result.setShape(result.fuse(sh));
        }
    }

    Shape.setValue(result);

    return App::DocumentObject::StdReturn;
}
