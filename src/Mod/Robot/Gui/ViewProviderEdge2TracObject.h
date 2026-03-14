#ifndef ROBOT_VIEWPROVIDEREDGE2TRACOBJECT_H
#define ROBOT_VIEWPROVIDEREDGE2TRACOBJECT_H

#include <Mod/Robot/RobotGlobal.h>

#include "ViewProviderTrajectory.h"

namespace RobotGui
{

class RobotGuiExport ViewProviderEdge2TracObject: public ViewProviderTrajectory
{
    PROPERTY_HEADER_WITH_OVERRIDE(RobotGui::ViewProviderEdge2TracObject);

public:
    bool doubleClicked() override;

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
};

}  // namespace RobotGui


#endif  // ROBOT_VIEWPROVIDEREDGE2TRACOBJECT_H
