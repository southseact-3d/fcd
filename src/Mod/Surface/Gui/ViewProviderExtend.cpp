#include <Gui/BitmapFactory.h>

#include "ViewProviderExtend.h"


using namespace SurfaceGui;

PROPERTY_SOURCE(SurfaceGui::ViewProviderExtend, PartGui::ViewProviderSpline)

namespace SurfaceGui
{

QIcon ViewProviderExtend::getIcon() const
{
    return Gui::BitmapFactory().pixmap("Surface_ExtendFace");
}

}  // namespace SurfaceGui
