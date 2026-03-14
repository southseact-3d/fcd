#ifndef ROBOT_ViewProviderTrajectoryDressUp_H
#define ROBOT_ViewProviderTrajectoryDressUp_H

#include <Mod/Robot/RobotGlobal.h>

#include "ViewProviderTrajectory.h"

namespace RobotGui
{

class RobotGuiExport ViewProviderTrajectoryDressUp: public ViewProviderTrajectory
{
    PROPERTY_HEADER_WITH_OVERRIDE(RobotGui::ViewProviderTrajectoryDressUp);

public:
    // virtual bool doubleClicked(void);

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
};

}  // namespace RobotGui


#endif  // ROBOT_ViewProviderTrajectoryDressUp_H
