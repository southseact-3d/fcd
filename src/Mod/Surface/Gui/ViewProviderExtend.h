#ifndef SURFACEGUI_VIEWPROVIDEREXTEND_H
#define SURFACEGUI_VIEWPROVIDEREXTEND_H

#include <Mod/Part/Gui/ViewProviderSpline.h>


namespace SurfaceGui
{

class ViewProviderExtend: public PartGui::ViewProviderSpline
{
    PROPERTY_HEADER_WITH_OVERRIDE(SurfaceGui::ViewProviderExtend);

public:
    QIcon getIcon() const override;
};

}  // namespace SurfaceGui

#endif  // SURFACEGUI_VIEWPROVIDEREXTEND_H
