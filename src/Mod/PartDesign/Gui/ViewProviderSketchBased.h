#ifndef VIEWPROVIDERSKETCHBASED_H_QKP3UG9A
#define VIEWPROVIDERSKETCHBASED_H_QKP3UG9A

#include "ViewProvider.h"

#include <Gui/ParamHandler.h>
#include <Gui/Inventor/SoToggleSwitch.h>

namespace PartDesignGui
{

/**
 * A common base class for Sketch based view providers
 */
class PartDesignGuiExport ViewProviderSketchBased: public ViewProvider
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderSketchBased);

public:
    /// constructor
    ViewProviderSketchBased();
    /// destructor
    ~ViewProviderSketchBased() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;

    void attach(App::DocumentObject* pcObject) override;

protected:
    void updateData(const App::Property* prop) override;
    void updatePreview() override;

private:
    void updateProfileShape();

    Gui::CoinPtr<SoToggleSwitch> pcProfileToggle;
    Gui::CoinPtr<PartGui::SoPreviewShape> pcProfileShape;

    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/PartDesign/Preview"
    );
    Gui::ParamHandlers handlers;
};

}  // namespace PartDesignGui

#endif /* end of include guard: VIEWPROVIDERSKETCHBASED_H_QKP3UG9A */
