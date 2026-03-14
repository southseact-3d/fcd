#include <Mod/Part/Gui/ViewProvider.h>
#include <Mod/Part/Gui/ViewProviderAttachExtension.h>
#include <Mod/Part/PartGlobal.h>


namespace PartGui
{

class PartGuiExport ViewProviderPrimitive: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderPrimitive);

public:
    /// constructor
    ViewProviderPrimitive();
    /// destructor
    ~ViewProviderPrimitive() override;

    void setupContextMenu(QMenu*, QObject*, const char*) override;

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;

private:
    ViewProviderAttachExtension extension;
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERPRIMITIVE_H
