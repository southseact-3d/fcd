#ifndef STYLEPARAMETERS_H
#define STYLEPARAMETERS_H

#include <Gui/StyleParameters/ParameterManager.h>

namespace PartDesignGui::StyleParameters
{
DEFINE_STYLE_PARAMETER(PreviewAdditiveColor, Base::Color(0.0F, 1.0F, 0.6F));
DEFINE_STYLE_PARAMETER(PreviewSubtractiveColor, Base::Color(1.0F, 0.0F, 0.0F));
DEFINE_STYLE_PARAMETER(PreviewCommonColor, Base::Color(1.0F, 1.0F, 0.0F));
DEFINE_STYLE_PARAMETER(PreviewDressUpColor, Base::Color(1.0F, 0.0F, 1.0F));

DEFINE_STYLE_PARAMETER(PreviewProfileLineWidth, Gui::StyleParameters::Numeric(4));
DEFINE_STYLE_PARAMETER(PreviewProfileOpacity, Gui::StyleParameters::Numeric(0.0));

DEFINE_STYLE_PARAMETER(PreviewErrorColor, Base::Color(1.0F, 0.0F, 0.0F));
DEFINE_STYLE_PARAMETER(PreviewErrorOpacity, Gui::StyleParameters::Numeric(0.05));

DEFINE_STYLE_PARAMETER(PreviewToolOpacity, Gui::StyleParameters::Numeric(0.05));
DEFINE_STYLE_PARAMETER(PreviewShapeOpacity, Gui::StyleParameters::Numeric(0.2));

DEFINE_STYLE_PARAMETER(PreviewLineWidth, Gui::StyleParameters::Numeric(2));
}  // namespace PartDesignGui::StyleParameters

#endif  // STYLEPARAMETERS_H
