#ifndef GUI_SIEMENSNXNAVIGATIONSTYLE_H
#define GUI_SIEMENSNXNAVIGATIONSTYLE_H

#include <Gui/Navigation/NavigationStateChart.h>

// NOLINTBEGIN(cppcoreguidelines-avoid*, readability-avoid-const-params-in-decls)
namespace Gui
{

class GuiExport SiemensNXNavigationStyle: public NavigationStateChart
{
    using inherited = NavigationStateChart;

    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    SiemensNXNavigationStyle();
    ~SiemensNXNavigationStyle() override;
    const char* mouseButtons(ViewerMode mode) override;
    std::string userFriendlyName() const override;

protected:
    SbBool processKeyboardEvent(const SoKeyboardEvent* const event) override;

private:
    struct NaviMachine;
    struct IdleState;
    struct AwaitingReleaseState;
    struct AwaitingMoveState;
    struct InteractState;
    struct RotateState;
    struct PanState;
    struct ZoomState;
    struct SelectionState;
};

}  // namespace Gui
// NOLINTEND(cppcoreguidelines-avoid*, readability-avoid-const-params-in-decls)

#endif  // GUI_SIEMENSNXNAVIGATIONSTYLE_H
