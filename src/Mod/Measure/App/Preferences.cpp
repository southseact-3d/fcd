#include <string>


#include <App/Application.h>
#include <App/Material.h>
#include <Base/Console.h>
#include <Base/Parameter.h>

#include "Preferences.h"


// getters for parameters used in multiple places.
// ensure this is in sync with parameter names and default values on preference pages

using namespace Measure;

//! Returns the Measure preference group
Base::Reference<ParameterGrp> Preferences::getPreferenceGroup(const char* Name)
{
    return App::GetApplication()
        .GetUserParameter()
        .GetGroup("BaseApp/Preferences/Mod/Measure")
        ->GetGroup(Name);
}

Base::Color Preferences::defaultLineColor()
{
    Base::Color fcColor;
    fcColor.setPackedValue(
        getPreferenceGroup("Appearance")->GetUnsigned("DefaultLineColor", 0x3CF00000)
    );
    return fcColor;
}

Base::Color Preferences::defaultTextColor()
{
    Base::Color fcColor;
    fcColor.setPackedValue(
        getPreferenceGroup("Appearance")->GetUnsigned("DefaultTextColor", 0x00000000)
    );
    return fcColor;
}

Base::Color Preferences::defaultTextBackgroundColor()
{
    Base::Color fcColor;
    fcColor.setPackedValue(
        getPreferenceGroup("Appearance")->GetUnsigned("DefaultTextBackgroundColor", 0x3CF00000)
    );
    return fcColor;
}

int Preferences::defaultFontSize()
{
    return getPreferenceGroup("Appearance")->GetInt("DefaultFontSize", 18);
}
