#ifndef GUI_VIEWPROVIDER_ViewProviderPart_H
#define GUI_VIEWPROVIDER_ViewProviderPart_H

#include "ActiveObjectList.h"
#include "ViewProviderGeometryObject.h"
#include "ViewProviderOriginGroup.h"
#include "ViewProviderFeaturePython.h"


namespace Gui
{

class GuiExport ViewProviderPart: public ViewProviderGeometryObject,
                                  public ViewProviderOriginGroupExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(Gui::ViewProviderPart);

public:
    /// constructor.
    ViewProviderPart();
    /// destructor.
    ~ViewProviderPart() override;

    bool doubleClicked() override;
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;
    bool isActivePart(const char* key = PARTKEY);
    void toggleActivePart();

    /// deliver the icon shown in the tree view
    /// override from ViewProvider.h
    QIcon getIcon() const override;

    /* Check whether the object accept reordering of its children during drop.*/
    bool acceptReorderingObjects() const override
    {
        return true;
    };

protected:
    /// get called by the container whenever a property has been changed
    void onChanged(const App::Property* prop) override;
    /// a second icon for the Assembly type
    const char* aPixmap;
};

using ViewProviderPartPython = ViewProviderFeaturePythonT<ViewProviderPart>;

}  // namespace Gui

#endif  // GUI_VIEWPROVIDER_ViewProviderPart_H
