#include "TrajectoryCompound.h"
#include "Waypoint.h"


using namespace Robot;
using namespace App;

PROPERTY_SOURCE(Robot::TrajectoryCompound, Robot::TrajectoryObject)


TrajectoryCompound::TrajectoryCompound()
{

    ADD_PROPERTY_TYPE(Source, (nullptr), "Compound", Prop_None, "list of trajectories to combine");
}

App::DocumentObjectExecReturn* TrajectoryCompound::execute()
{
    const std::vector<DocumentObject*>& Tracs = Source.getValues();
    Robot::Trajectory result;

    for (auto it : Tracs) {
        if (it->isDerivedFrom<Robot::TrajectoryObject>()) {
            const std::vector<Waypoint*>& wps
                = static_cast<Robot::TrajectoryObject*>(it)->Trajectory.getValue().getWaypoints();
            for (auto wp : wps) {
                result.addWaypoint(*wp);
            }
        }
        else {
            return new App::DocumentObjectExecReturn("Not all objects in compound are trajectories!");
        }
    }

    Trajectory.setValue(result);

    return App::DocumentObject::StdReturn;
}


// short TrajectoryCompound::mustExecute(void) const
//{
//     return 0;
// }

// void TrajectoryCompound::onChanged(const Property* prop)
//{
//
//     App::GeoFeature::onChanged(prop);
// }
