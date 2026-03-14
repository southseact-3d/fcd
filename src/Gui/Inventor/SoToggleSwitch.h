#ifndef SO_TOGGLE_SWITCH
#define SO_TOGGLE_SWITCH

#include <FCGlobal.h>

#include <Inventor/fields/SoSFBool.h>
#include <Inventor/nodes/SoSwitch.h>

#include <FCGlobal.h>

/**
 * A switch that can be used to show or hide all child nodes
 */
class GuiExport SoToggleSwitch: public SoSwitch
{
    SO_NODE_HEADER(SoToggleSwitch);

public:
    static void initClass();
    SoToggleSwitch();

    // the switch is on be default
    SoSFBool on;
    // toggles the switch state
    void toggle();

protected:
    ~SoToggleSwitch() override = default;

    void notify(SoNotList* notList) override;

private:
    using inherited = SoSwitch;
};

#endif /* SO_TOGGLE_SWITCH */
