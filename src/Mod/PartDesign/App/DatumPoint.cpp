#include <BRepBuilderAPI_MakeVertex.hxx>
#include <Standard_Version.hxx>


#include "DatumPoint.h"

using namespace PartDesign;
using namespace Attacher;

// ============================================================================

PROPERTY_SOURCE(PartDesign::Point, Part::Datum)

Point::Point()
{
    this->setAttacher(new AttachEnginePoint);
    this->makeShape();
}

Point::~Point() = default;

void Point::onChanged(const App::Property* prop)
{
    if (prop == &(this->Shape)) {
        // fix for #0002758 Datum point moves to (0,0,0) when reopening the file.
        // bypass Part::Feature's onChanged, which may alter Placement property to match shape's
        // placement. This is to prevent loss of correct Placement when restoring Shape from file.
        App::GeoFeature::onChanged(prop);
        return;
    }
    Superclass::onChanged(prop);
}

void Point::onDocumentRestored()
{
    // fix for #0002758 Datum point moves to (0,0,0) when reopening the file.
    // recreate shape, as the restored one has old Placement burned into it.
    this->makeShape();
    Superclass::onDocumentRestored();
}

void Point::makeShape()
{
    // Create a shape, which will be used by Sketcher, attachables, and whatever. Them main function
    // is to avoid a dependency of Sketcher on the PartDesign module
    BRepBuilderAPI_MakeVertex builder(gp_Pnt(0, 0, 0));
    if (!builder.IsDone()) {
        return;
    }
    Part::TopoShape tshape(builder.Shape());
    tshape.setPlacement(this->Placement.getValue());
    Shape.setValue(tshape);
}

Base::Vector3d Point::getPoint()
{
    return Placement.getValue().getPosition();
}
