#ifndef FEMGUI_SETTINGS_H
#define FEMGUI_SETTINGS_H

#include <Base/Parameter.h>

namespace FemGui
{

class FemSettings
{
public:
    FemSettings();
    void setPostAutoRecompute(bool);
    bool getPostAutoRecompute() const;

private:
    ParameterGrp::handle pGroup;
};

}  // namespace FemGui

#endif  // FEMGUI_SETTINGS_H
