#include "SoToggleSwitch.h"

SO_NODE_SOURCE(SoToggleSwitch)

void SoToggleSwitch::initClass()
{
    SO_NODE_INIT_CLASS(SoToggleSwitch, SoSwitch, "Switch");
}

SoToggleSwitch::SoToggleSwitch()
{
    SO_NODE_CONSTRUCTOR(SoToggleSwitch);
    SO_NODE_ADD_FIELD(on, (1));

    whichChild = SO_SWITCH_ALL;
}

void SoToggleSwitch::toggle()
{
    on = !on.getValue();
}

void SoToggleSwitch::notify(SoNotList* notList)
{
    assert(notList);
    if (notList->getLastField() == &on) {
        whichChild = on.getValue() ? SO_SWITCH_ALL : SO_SWITCH_NONE;
    }

    inherited::notify(notList);
}
