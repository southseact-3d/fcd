#ifndef GUI_GIZMO_STYLE_PARAMETERS_H
#define GUI_GIZMO_STYLE_PARAMETERS_H

#include <Base/Builder3D.h>
#include <Gui/StyleParameters/ParameterManager.h>

namespace Gui::StyleParameters
{
DEFINE_STYLE_PARAMETER(LinearGizmoBaseColor, Base::Color(1.0F, 0.0F, 0.0F));
DEFINE_STYLE_PARAMETER(LinearGizmoActiveColor, Base::Color(1.0F, 1.0F, 0.0F));

DEFINE_STYLE_PARAMETER(RotationGizmoBaseColor, Base::Color(1.0F, 0.0F, 0.0F));
DEFINE_STYLE_PARAMETER(RotationGizmoActiveColor, Base::Color(1.0F, 1.0F, 0.0F));

DEFINE_STYLE_PARAMETER(DimensionVisualizerColor, Base::Color(0.214F, 0.560F, 0.930F));
}  // namespace Gui::StyleParameters

#endif /* GUI_GIZMO_STYLE_PARAMETERS_H */
