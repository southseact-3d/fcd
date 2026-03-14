#ifndef PARTGUI_VIEWPROVIDERPROJECTONSURFACE_H
#define PARTGUI_VIEWPROVIDERPROJECTONSURFACE_H

#include <Mod/Part/PartGlobal.h>

#include <Mod/Part/Gui/ViewProvider.h>


namespace PartGui
{


class PartGuiExport ViewProviderProjectOnSurface: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderProjectOnSurface);

public:
    /// constructor
    ViewProviderProjectOnSurface();
    /// destructor
    ~ViewProviderProjectOnSurface() override;

    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERPROJECTONSURFACE_H
