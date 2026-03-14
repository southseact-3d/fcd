#ifndef ROBOT_ViewProviderTrajectoryCompound_H
#define ROBOT_ViewProviderTrajectoryCompound_H

#include <Mod/Robot/RobotGlobal.h>

#include "ViewProviderTrajectory.h"

namespace RobotGui
{

class RobotGuiExport ViewProviderTrajectoryCompound: public ViewProviderTrajectory
{
    PROPERTY_HEADER_WITH_OVERRIDE(RobotGui::ViewProviderTrajectoryCompound);

public:
    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
};

}  // namespace RobotGui


#endif  // ROBOT_ViewProviderTrajectoryCompound_H
