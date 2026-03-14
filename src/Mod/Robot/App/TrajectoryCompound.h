#ifndef ROBOT_TrajectoryCompound_H
#define ROBOT_TrajectoryCompound_H

#include <App/PropertyLinks.h>

#include <Mod/Robot/RobotGlobal.h>

#include "TrajectoryObject.h"


namespace Robot
{

class RobotExport TrajectoryCompound: public TrajectoryObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Robot::TrajectoryObject);

public:
    /// Constructor
    TrajectoryCompound();

    App::PropertyLinkList Source;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "RobotGui::ViewProviderTrajectoryCompound";
    }
    App::DocumentObjectExecReturn* execute() override;

protected:
    /// get called by the container when a property has changed
    // virtual void onChanged (const App::Property* prop);
};

}  // namespace Robot


#endif  // ROBOT_ROBOTOBJECT_H
