#ifndef ROBOT_Edge2TracObject_H
#define ROBOT_Edge2TracObject_H

#include <App/PropertyGeo.h>

#include <Mod/Robot/RobotGlobal.h>

#include "TrajectoryObject.h"


namespace Robot
{

class RobotExport Edge2TracObject: public TrajectoryObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Robot::TrajectoryObject);

public:
    /// Constructor
    Edge2TracObject();

    App::PropertyLinkSub Source;
    App::PropertyFloatConstraint SegValue;
    App::PropertyBool UseRotation;

    /// set by execute with the number of clusters found
    int NbrOfCluster;
    /// set by execute with the number of all edges
    int NbrOfEdges;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "RobotGui::ViewProviderEdge2TracObject";
    }
    App::DocumentObjectExecReturn* execute() override;

protected:
    /// get called by the container when a property has changed
    void onChanged(const App::Property* prop) override;
};

}  // namespace Robot


#endif  // ROBOT_ROBOTOBJECT_H
