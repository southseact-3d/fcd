#ifndef GUI_ViewProviderLine_H
#define GUI_ViewProviderLine_H

#include "ViewProviderDatum.h"

#include "ParamHandler.h"

class SoCoordinate3;
class SoTranslation;

namespace Gui
{
class SoFrameLabel;
}

namespace Gui
{

class GuiExport ViewProviderLine: public ViewProviderDatum
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderLine);

public:
    /// Constructor
    ViewProviderLine();
    ~ViewProviderLine() override;

    void attach(App::DocumentObject*) override;

private:
    void updateLineSize();

protected:
    CoinPtr<SoFrameLabel> pLabel;
    CoinPtr<SoCoordinate3> pCoords;
    CoinPtr<SoTranslation> pLabelTranslation;

    ParamHandlers handlers;
};

}  // namespace Gui


#endif  // GUI_ViewProviderLine_H
