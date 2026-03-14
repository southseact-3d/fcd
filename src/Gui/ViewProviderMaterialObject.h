#ifndef GUI_ViewProviderMaterialObject_H
#define GUI_ViewProviderMaterialObject_H


#include "ViewProviderDocumentObject.h"
#include "ViewProviderFeaturePython.h"

namespace Gui
{

class GuiExport ViewProviderMaterialObject: public ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderMaterialObject);

public:
    /// constructor.
    ViewProviderMaterialObject();
    /// destructor.
    ~ViewProviderMaterialObject() override;

    QIcon getIcon() const override;

    bool doubleClicked() override;

    // shows solid in the tree
    bool isShow() const override
    {
        return true;
    }
};

using ViewProviderMaterialObjectPython = ViewProviderFeaturePythonT<ViewProviderMaterialObject>;

}  // namespace Gui

#endif  // GUI_ViewProviderMaterialObject_H
