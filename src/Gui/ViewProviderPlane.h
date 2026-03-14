#ifndef GUI_ViewProviderPlane_H
#define GUI_ViewProviderPlane_H

#include "ViewProviderDatum.h"

#include "Selection/Selection.h"
#include "ParamHandler.h"


class SoSwitch;
class SoTranslation;
class SoAsciiText;
class SoCoordinate3;

namespace Gui
{

class GuiExport ViewProviderPlane: public ViewProviderDatum, public SelectionObserver
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderPlane);

public:
    /// Constructor
    ViewProviderPlane();
    ~ViewProviderPlane() override;

    void attach(App::DocumentObject*) override;

    unsigned long getColor(const std::string& role) const;
    std::string getRole() const;
    std::string getLabelText(const std::string& role) const;
    void setLabelVisibility(bool val);

    void onSelectionChanged(const SelectionChanges&) override;

private:
    void updatePlaneSize();

    bool isHovered {false};
    bool isSelected {false};

    CoinPtr<SoSwitch> labelSwitch;
    CoinPtr<SoAsciiText> pLabel;
    CoinPtr<SoCoordinate3> pCoords;
    CoinPtr<SoTranslation> pTextTranslation;

    ParamHandlers handlers;
};

}  // namespace Gui


#endif  // GUI_ViewProviderPlane_H
