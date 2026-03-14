#ifndef GUI_VIEWPROVIDER_DOCUMENTOBJECTGROUP_H
#define GUI_VIEWPROVIDER_DOCUMENTOBJECTGROUP_H

#include "ViewProviderDocumentObject.h"
#include "ViewProviderGroupExtension.h"
#include "ViewProviderFeaturePython.h"


namespace Gui
{

class GuiExport ViewProviderDocumentObjectGroup: public ViewProviderDocumentObject,
                                                 public ViewProviderGroupExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(Gui::ViewProviderDocumentObjectGroup);

public:
    /// constructor.
    ViewProviderDocumentObjectGroup();
    /// destructor.
    ~ViewProviderDocumentObjectGroup() override;

    /// returns a list of all possible modes
    std::vector<std::string> getDisplayModes() const override;
    bool isShow() const override;

    /// deliver the icon shown in the tree view
    QIcon getIcon() const override;

    // Set up the context menu with the supported edit modes
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;

    /* Check whether the object accept reordering of its children during drop.*/
    bool acceptReorderingObjects() const override
    {
        return true;
    };

protected:
    void getViewProviders(std::vector<ViewProviderDocumentObject*>&) const;

private:
    std::vector<ViewProvider*> nodes;
};

using ViewProviderDocumentObjectGroupPython
    = ViewProviderFeaturePythonT<ViewProviderDocumentObjectGroup>;

}  // namespace Gui

#endif  // GUI_VIEWPROVIDER_DOCUMENTOBJECTGROUP_H
