#ifndef ASSEMBLYGUI_VIEWPROVIDER_ViewProviderBomGroup_H
#define ASSEMBLYGUI_VIEWPROVIDER_ViewProviderBomGroup_H

#include <Mod/Assembly/AssemblyGlobal.h>

#include <Gui/ViewProviderDocumentObjectGroup.h>


namespace AssemblyGui
{

class AssemblyGuiExport ViewProviderBomGroup: public Gui::ViewProviderDocumentObjectGroup
{
    PROPERTY_HEADER_WITH_OVERRIDE(AssemblyGui::ViewProviderBomGroup);

public:
    ViewProviderBomGroup();
    ~ViewProviderBomGroup() override;

    /// deliver the icon shown in the tree view. Override from ViewProvider.h
    QIcon getIcon() const override;

    // Prevent dragging of the joints and dropping things inside the joint group.
    bool canDragObjects() const override
    {
        return false;
    };
    bool canDropObjects() const override
    {
        return false;
    };
    bool canDragAndDropObject(App::DocumentObject*) const override
    {
        return false;
    };

    // protected:
    /// get called by the container whenever a property has been changed
    // void onChanged(const App::Property* prop) override;
};

}  // namespace AssemblyGui

#endif  // ASSEMBLYGUI_VIEWPROVIDER_ViewProviderBomGroup_H
