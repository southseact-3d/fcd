#ifndef SURFACEGUI_VIEWPROVIDERBLENDCURVE_H
#define SURFACEGUI_VIEWPROVIDERBLENDCURVE_H

#include <Mod/Part/Gui/ViewProviderSpline.h>

namespace SurfaceGui
{

class ViewProviderBlendCurve: public PartGui::ViewProviderSpline
{
    PROPERTY_HEADER_WITH_OVERRIDE(SurfaceGui::ViewProviderBlendCurve);

public:
    QIcon getIcon() const override;
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
};

}  // namespace SurfaceGui

#endif  // SURFACEGUI_VIEWPROVIDEREXTEND_H
