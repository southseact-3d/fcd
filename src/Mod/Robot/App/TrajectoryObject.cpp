#include <App/DocumentObjectPy.h>
#include <Base/Placement.h>

#include "TrajectoryObject.h"


using namespace Robot;
using namespace App;

PROPERTY_SOURCE(Robot::TrajectoryObject, App::GeoFeature)


TrajectoryObject::TrajectoryObject()
{

    ADD_PROPERTY_TYPE(Base, (Base::Placement()), "Trajectory", Prop_None, "Base frame of the trajectory");
    ADD_PROPERTY_TYPE(Trajectory, (Robot::Trajectory()), "Trajectory", Prop_None, "Trajectory object");
}

short TrajectoryObject::mustExecute() const
{
    return 0;
}

PyObject* TrajectoryObject::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new DocumentObjectPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}

void TrajectoryObject::onChanged(const Property* prop)
{
    App::GeoFeature::onChanged(prop);
}
