#ifndef GUI_ViewProviderVarSet_H
#define GUI_ViewProviderVarSet_H

#include "ViewProviderDocumentObject.h"
#include "Dialogs/DlgAddProperty.h"

namespace Gui
{

/** View provider associated with an App::VarSet
 */
class GuiExport ViewProviderVarSet: public ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderVarSet);

public:
    ViewProviderVarSet();
    ~ViewProviderVarSet() override = default;

    bool isShow() const override
    {
        return true;
    }

    bool doubleClicked() override;

    void onFinished(int);

private:
    std::unique_ptr<Dialog::DlgAddProperty> dialog;
};

}  // namespace Gui

#endif
