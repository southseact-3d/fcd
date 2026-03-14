#ifndef ROBOT_ViewProviderTrajectory_H
#define ROBOT_ViewProviderTrajectory_H

#include <Gui/Selection/SoFCSelection.h>
#include <Gui/ViewProviderGeometryObject.h>
#include <Mod/Robot/RobotGlobal.h>


class SoCoordinate3;
class SoDragger;
class SoDrawStyle;
class SoJackDragger;
class SoLineSet;

namespace RobotGui
{

class RobotGuiExport ViewProviderTrajectory: public Gui::ViewProviderGeometryObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(RobotGui::ViewProviderTrajectory);

public:
    /// constructor.
    ViewProviderTrajectory();

    /// destructor.
    ~ViewProviderTrajectory() override;

    void attach(App::DocumentObject* pcObject) override;
    void setDisplayMode(const char* ModeName) override;
    std::vector<std::string> getDisplayModes() const override;
    void updateData(const App::Property*) override;
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;

protected:
    Gui::SoFCSelection* pcTrajectoryRoot;
    SoCoordinate3* pcCoords;
    SoDrawStyle* pcDrawStyle;
    SoLineSet* pcLines;
};

}  // namespace RobotGui


#endif  // ROBOT_VIEWPROVIDERROBOTOBJECT_H
