#include <BRepBuilderAPI_MakeEdge.hxx>
#include <gp_Circ.hxx>
#include <TopoDS_Edge.hxx>


#include <Base/Tools.h>

#include "FeaturePartCircle.h"


using namespace Part;

App::PropertyQuantityConstraint::Constraints Circle::angleRange = {0.0, 360.0, 1.0};

PROPERTY_SOURCE(Part::Circle, Part::Primitive)


Circle::Circle()
{
    ADD_PROPERTY(Radius, (2.0f));
    ADD_PROPERTY(Angle1, (0.0f));
    Angle1.setConstraints(&angleRange);
    ADD_PROPERTY(Angle2, (360.0f));
    Angle2.setConstraints(&angleRange);
}

Circle::~Circle() = default;

short Circle::mustExecute() const
{
    if (Angle1.isTouched() || Angle2.isTouched() || Radius.isTouched()) {
        return 1;
    }
    return Part::Primitive::mustExecute();
}

App::DocumentObjectExecReturn* Circle::execute()
{
    gp_Circ circle;
    circle.SetRadius(this->Radius.getValue());

    BRepBuilderAPI_MakeEdge clMakeEdge(
        circle,
        Base::toRadians<double>(this->Angle1.getValue()),
        Base::toRadians<double>(this->Angle2.getValue())
    );
    const TopoDS_Edge& edge = clMakeEdge.Edge();
    this->Shape.setValue(edge);
    return Primitive::execute();
}

void Circle::onChanged(const App::Property* prop)
{
    if (!isRestoring()) {
        if (prop == &Radius || prop == &Angle1 || prop == &Angle2) {
            try {
                App::DocumentObjectExecReturn* ret = recompute();
                delete ret;
            }
            catch (...) {
            }
        }
    }
    Part::Feature::onChanged(prop);  // clazy:exclude=skipped-base-method
}

void Circle::Restore(Base::XMLReader& reader)
{
    Base::ObjectStatusLocker<App::Property::Status, App::Property> lock(
        App::Property::User1,
        &Angle2,
        false
    );
    Primitive::Restore(reader);

    if (Angle2.testStatus(App::Property::User1)) {
        double tmp = Angle1.getValue();
        Angle1.setValue(Angle2.getValue());
        Angle2.setValue(tmp);
    }
}

void Circle::handleChangedPropertyName(Base::XMLReader& reader, const char* TypeName, const char* PropName)
{
    Base::Type type = Base::Type::fromName(TypeName);
    if (Angle2.getTypeId() == type && strcmp(PropName, "Angle0") == 0) {
        Angle2.Restore(reader);
        // set the flag to swap Angle1/Angle2 afterwards
        Angle2.setStatus(App::Property::User1, true);
    }
    else {
        Primitive::handleChangedPropertyName(reader, TypeName, PropName);
    }
}
