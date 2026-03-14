#ifndef ROBOT_TrajectoryObject_H
#define ROBOT_TrajectoryObject_H

#include <App/GeoFeature.h>
#include <App/PropertyGeo.h>

#include <Mod/Robot/RobotGlobal.h>

#include "PropertyTrajectory.h"
#include "Trajectory.h"


namespace Robot
{

class RobotExport TrajectoryObject: public App::GeoFeature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Robot::TrajectoryObject);

public:
    /// Constructor
    TrajectoryObject();

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "RobotGui::ViewProviderTrajectory";
    }
    App::DocumentObjectExecReturn* execute() override
    {
        return App::DocumentObject::StdReturn;
    }
    short mustExecute() const override;
    PyObject* getPyObject() override;

    App::PropertyPlacement Base;
    PropertyTrajectory Trajectory;


protected:
    /// get called by the container when a property has changed
    void onChanged(const App::Property* prop) override;
};

}  // namespace Robot


#endif  // ROBOT_ROBOTOBJECT_H
