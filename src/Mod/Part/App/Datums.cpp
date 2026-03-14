#include "Datums.h"


using namespace Part;
using namespace Attacher;

PROPERTY_SOURCE_WITH_EXTENSIONS(Part::DatumPlane, App::Plane)

Part::DatumPlane::DatumPlane()
{
    AttachExtension::initExtension(this);
    this->setAttacher(new AttachEnginePlane);
}


PROPERTY_SOURCE_WITH_EXTENSIONS(Part::DatumLine, App::Line)

Part::DatumLine::DatumLine()
{
    setBaseDirection(Base::Vector3d(0, 0, 1));
    AttachExtension::initExtension(this);
    this->setAttacher(new AttachEngineLine);
}


PROPERTY_SOURCE_WITH_EXTENSIONS(Part::DatumPoint, App::Point)

Part::DatumPoint::DatumPoint()
{
    AttachExtension::initExtension(this);
    this->setAttacher(new AttachEnginePoint);
}


PROPERTY_SOURCE_WITH_EXTENSIONS(Part::LocalCoordinateSystem, App::LocalCoordinateSystem)

Part::LocalCoordinateSystem::LocalCoordinateSystem()
{
    AttachExtension::initExtension(this);
}
