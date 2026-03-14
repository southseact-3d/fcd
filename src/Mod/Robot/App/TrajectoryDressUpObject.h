#ifndef ROBOT_TrajectoryDressUpObject_H
#define ROBOT_TrajectoryDressUpObject_H

#include <App/PropertyGeo.h>
#include <App/PropertyUnits.h>

#include <Mod/Robot/RobotGlobal.h>

#include "TrajectoryObject.h"


namespace Robot
{

class RobotExport TrajectoryDressUpObject: public TrajectoryObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Robot::TrajectoryObject);

public:
    /// Constructor
    TrajectoryDressUpObject();

    App::PropertyLink Source;
    App::PropertySpeed Speed;
    App::PropertyBool UseSpeed;
    App::PropertyAcceleration Acceleration;
    App::PropertyBool UseAcceleration;
    App::PropertyEnumeration ContType;
    App::PropertyPlacement PosAdd;
    App::PropertyEnumeration AddType;


    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "RobotGui::ViewProviderTrajectoryDressUp";
    }
    App::DocumentObjectExecReturn* execute() override;

    static const char* ContTypeEnums[];
    static const char* AddTypeEnums[];

protected:
    /// get called by the container when a property has changed
    void onChanged(const App::Property* prop) override;
};

}  // namespace Robot


#endif  // ROBOT_ROBOTOBJECT_H
