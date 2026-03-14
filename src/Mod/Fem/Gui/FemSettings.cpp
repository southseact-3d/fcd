#include <App/Application.h>

#include "FemSettings.h"


using namespace FemGui;


FemSettings::FemSettings()
{
    pGroup = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Fem"
    );
}

void FemSettings::setPostAutoRecompute(bool on)
{
    pGroup->SetBool("PostAutoRecompute", on);
}

bool FemSettings::getPostAutoRecompute() const
{
    return pGroup->GetBool("PostAutoRecompute", true);
}
