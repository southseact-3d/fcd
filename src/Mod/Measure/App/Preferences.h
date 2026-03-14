#ifndef Preferences_h_
#define Preferences_h_

#include <Mod/Measure/MeasureGlobal.h>

#include <string>

#include <Base/Parameter.h>
#include <App/Material.h>

namespace App
{
class Color;
}

namespace Measure
{

// getters for parameters used in multiple places.
class MeasureExport Preferences
{

public:
    static Base::Reference<ParameterGrp> getPreferenceGroup(const char* Name);

    static Base::Color defaultLineColor();
    static Base::Color defaultTextColor();
    static int defaultFontSize();
    static Base::Color defaultTextBackgroundColor();
};


}  // end namespace Measure
#endif
