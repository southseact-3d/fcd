#ifndef GUI_ViewProviderPoint_H
#define GUI_ViewProviderPoint_H

#include "ViewProviderDatum.h"

namespace Gui
{

class GuiExport ViewProviderPoint: public ViewProviderDatum
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderPoint);

public:
    /// Constructor
    ViewProviderPoint();
    ~ViewProviderPoint() override;

    void attach(App::DocumentObject*) override;
};

}  // namespace Gui


#endif  // GUI_ViewProviderPoint_H
