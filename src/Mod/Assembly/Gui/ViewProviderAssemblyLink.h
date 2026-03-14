#ifndef ASSEMBLYGUI_VIEWPROVIDER_ViewProviderAssemblyLink_H
#define ASSEMBLYGUI_VIEWPROVIDER_ViewProviderAssemblyLink_H

#include <QCoreApplication>

#include <Mod/Assembly/AssemblyGlobal.h>

#include <Gui/ViewProviderPart.h>


namespace AssemblyGui
{

class AssemblyGuiExport ViewProviderAssemblyLink: public Gui::ViewProviderPart
{
    Q_DECLARE_TR_FUNCTIONS(AssemblyGui::ViewProviderAssemblyLink)
    PROPERTY_HEADER_WITH_OVERRIDE(AssemblyGui::ViewProviderAssemblyLink);

public:
    ViewProviderAssemblyLink();
    ~ViewProviderAssemblyLink() override;

    /// deliver the icon shown in the tree view. Override from ViewProvider.h
    QIcon getIcon() const override;

    bool setEdit(int ModNum) override;

    bool doubleClicked() override;

    // When the assembly link is deleted, we delete all its content as well.
    bool onDelete(const std::vector<std::string>& subNames) override;

    // Prevent deletion of the link assembly's content.
    bool canDelete(App::DocumentObject*) const override
    {
        return false;
    };

    // Prevent drag/drop of objects within the assembly link.
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

    void setupContextMenu(QMenu*, QObject*, const char*) override;
};

}  // namespace AssemblyGui

#endif  // ASSEMBLYGUI_VIEWPROVIDER_ViewProviderAssemblyLink_H
