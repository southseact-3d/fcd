#ifndef DRAWINGGUI_VIEWPROVIDERBALLOON_H
#define DRAWINGGUI_VIEWPROVIDERBALLOON_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <App/PropertyUnits.h>
#include <Mod/TechDraw/App/DrawViewBalloon.h>

#include "ViewProviderDrawingView.h"


namespace TechDrawGui {


class TechDrawGuiExport ViewProviderBalloon : public ViewProviderDrawingView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderBalloon);

public:
    /// constructor
    ViewProviderBalloon();
    /// destructor
    ~ViewProviderBalloon() override = default;

    App::PropertyFont   Font;
    App::PropertyLength Fontsize;
    App::PropertyLength LineWidth;
    App::PropertyBool   LineVisible;
    App::PropertyColor  Color;

    bool useNewSelectionModel() const override {return false;}
    void updateData(const App::Property* prop) override;
    void onChanged(const App::Property* prop) override;
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;
    bool setEdit(int ModNum) override;
    bool doubleClicked() override;
    bool canDelete(App::DocumentObject* obj) const override;
    bool onDelete(const std::vector<std::string> & parms) override;

    TechDraw::DrawViewBalloon* getViewObject() const override;

    std::vector<App::DocumentObject*> claimChildren() const override;

protected:
    void handleChangedPropertyType(Base::XMLReader &reader, const char *TypeName, App::Property * prop) override;
};

} // namespace TechDrawGui


#endif // DRAWINGGUI_VIEWPROVIDERBALLOON_H
