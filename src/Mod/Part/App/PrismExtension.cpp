#include <BRepPrimAPI_MakePrism.hxx>


#include <Base/Tools.h>

#include "PrismExtension.h"


using namespace Part;

EXTENSION_PROPERTY_SOURCE(Part::PrismExtension, App::DocumentObjectExtension)

PrismExtension::PrismExtension()
{
    EXTENSION_ADD_PROPERTY_TYPE(FirstAngle, (0.0f), "Prism", App::Prop_None, "Angle in first direction");
    EXTENSION_ADD_PROPERTY_TYPE(SecondAngle, (0.0f), "Prism", App::Prop_None, "Angle in second direction");

    static const App::PropertyQuantityConstraint::Constraints angleConstraint
        = {-89.99999, 89.99999, 1.0};
    FirstAngle.setConstraints(&angleConstraint);
    SecondAngle.setConstraints(&angleConstraint);

    initExtensionType(PrismExtension::getExtensionClassTypeId());
}

PrismExtension::~PrismExtension() = default;

short int PrismExtension::extensionMustExecute()
{
    if (FirstAngle.isTouched()) {
        return 1;
    }
    if (SecondAngle.isTouched()) {
        return 1;
    }
    return DocumentObjectExtension::extensionMustExecute();
}

App::DocumentObjectExecReturn* PrismExtension::extensionExecute()
{
    return App::DocumentObjectExtension::extensionExecute();
}

void PrismExtension::extensionOnChanged(const App::Property* prop)
{
    App::DocumentObjectExtension::extensionOnChanged(prop);
}

TopoDS_Shape PrismExtension::makePrism(double height, const TopoDS_Face& face) const
{
    // the direction vector for the prism is the height for z and the given angle
    BRepPrimAPI_MakePrism mkPrism(
        face,
        gp_Vec(
            height * tan(Base::toRadians<double>(FirstAngle.getValue())),
            height * tan(Base::toRadians<double>(SecondAngle.getValue())),
            height
        )
    );
    return mkPrism.Shape();
}
