#ifndef PARTGUI_ViewProviderBase_H
#define PARTGUI_ViewProviderBase_H

#include "ViewProvider.h"

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderBase: public ViewProvider
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderBase);

public:
    /// constructor
    ViewProviderBase();
    /// destructor
    ~ViewProviderBase() override;

    bool doubleClicked() override;
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;

    Gui::ViewProvider* startEditing(int ModNum) override;

protected:
    bool setEdit(int ModNum) override;
};

}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderBase_H
