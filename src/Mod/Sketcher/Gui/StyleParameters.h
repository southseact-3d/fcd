#ifndef SKETCHER_STYLEPARAMETERS_H
#define SKETCHER_STYLEPARAMETERS_H

#include <Gui/StyleParameters/ParameterManager.h>

namespace SketcherGui::StyleParameters
{
// rubberband selection colors
DEFINE_STYLE_PARAMETER(
    SketcherRubberbandTouchSelectionColor,
    Base::Color(0.0F, 1.0F, 0.0F, 1.0F)
);  // green for touch selection (right to left)
DEFINE_STYLE_PARAMETER(
    SketcherRubberbandWindowSelectionColor,
    Base::Color(0.0F, 0.4F, 1.0F, 1.0F)
);  // blue for window selection (left to right)
}  // namespace SketcherGui::StyleParameters

#endif  // SKETCHER_STYLEPARAMETERS_H
