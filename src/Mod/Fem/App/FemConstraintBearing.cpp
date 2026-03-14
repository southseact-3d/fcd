#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TopoDS.hxx>


#include <Mod/Part/App/PartFeature.h>

#include "FemConstraintBearing.h"
#include "FemTools.h"

using namespace Fem;

PROPERTY_SOURCE(Fem::ConstraintBearing, Fem::Constraint)

ConstraintBearing::ConstraintBearing()
{
    ADD_PROPERTY_TYPE(
        Location,
        (nullptr),
        "ConstraintBearing",
        (App::PropertyType)(App::Prop_None),
        "Element giving axial location of constraint"
    );
    ADD_PROPERTY(Dist, (0.0));
    ADD_PROPERTY(AxialFree, (0));
    ADD_PROPERTY(Radius, (0.0));
    ADD_PROPERTY(Height, (0.0));
    ADD_PROPERTY_TYPE(
        BasePoint,
        (Base::Vector3d(0, 0, 0)),
        "ConstraintBearing",
        App::PropertyType(App::Prop_ReadOnly | App::Prop_Output),
        "Base point of cylindrical bearing seat"
    );
    ADD_PROPERTY_TYPE(
        Axis,
        (Base::Vector3d(0, 1, 0)),
        "ConstraintBearing",
        App::PropertyType(App::Prop_ReadOnly | App::Prop_Output),
        "Axis of bearing seat"
    );
}

App::DocumentObjectExecReturn* ConstraintBearing::execute()
{
    return Constraint::execute();
}

void ConstraintBearing::onChanged(const App::Property* prop)
{
    // Note: If we call this at the end, then the symbol is not oriented correctly initially
    // because the NormalDirection has not been calculated yet
    Constraint::onChanged(prop);

    if (prop == &References) {
        // Find data of cylinder
        std::vector<App::DocumentObject*> ref = References.getValues();
        std::vector<std::string> subRef = References.getSubValues();
        if (ref.empty()) {
            return;
        }

        Part::Feature* feat = static_cast<Part::Feature*>(ref.front());
        TopoDS_Shape sh = Tools::getFeatureSubShape(feat, subRef.front().c_str(), true);
        double radius, height;
        Base::Vector3d base, axis;
        if (sh.IsNull() || !Tools::getCylinderParams(sh, base, axis, height, radius)) {
            return;
        }
        Radius.setValue(radius);
        Axis.setValue(axis);
        Height.setValue(height);
        // Update base point
        base = base + axis * height / 2;
        if (Location.getValue()) {
            base = getBasePoint(base, axis, Location, Dist.getValue());
        }
        BasePoint.setValue(base);
        BasePoint.touch();  // This triggers ViewProvider::updateData()
    }
    else if ((prop == &Location) || (prop == &Dist)) {
        App::DocumentObject* obj = Location.getValue();
        std::vector<std::string> names = Location.getSubValues();
        if (names.empty()) {
            return;
        }
        std::string subName = names.front();
        Part::Feature* feat = static_cast<Part::Feature*>(obj);
        TopoDS_Shape sh = feat->Shape.getShape().getSubShape(subName.c_str());

        if (sh.ShapeType() == TopAbs_FACE) {
            BRepAdaptor_Surface surface(TopoDS::Face(sh));
            if (surface.GetType() != GeomAbs_Plane) {
                return;  // "Location must be a planar face or linear edge"
            }
        }
        else if (sh.ShapeType() == TopAbs_EDGE) {
            BRepAdaptor_Curve line(TopoDS::Edge(sh));
            if (line.GetType() != GeomAbs_Line) {
                return;  // "Location must be a planar face or linear edge"
            }
        }

        std::vector<App::DocumentObject*> ref = References.getValues();
        std::vector<std::string> subRef = References.getSubValues();
        if (ref.empty()) {
            return;
        }

        feat = static_cast<Part::Feature*>(ref.front());
        sh = Tools::getFeatureSubShape(feat, subRef.front().c_str(), true);
        double radius, height;
        Base::Vector3d base, axis;
        if (!Tools::getCylinderParams(sh, base, axis, height, radius)) {
            return;
        }
        BasePoint.setValue(base);
        BasePoint.touch();
    }
}
