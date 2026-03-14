#ifndef DRAWINGGUI_VIEWPROVIDERCLIP_H
#define DRAWINGGUI_VIEWPROVIDERCLIP_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <Mod/TechDraw/App/DrawViewClip.h>

#include "ViewProviderDrawingView.h"


namespace TechDrawGui {

class TechDrawGuiExport ViewProviderViewClip : public ViewProviderDrawingView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderViewClip);

public:
    /// constructor
    ViewProviderViewClip();
    /// destructor
    ~ViewProviderViewClip() override;

    App::PropertyBool   ClipChildren;

    bool useNewSelectionModel() const override {return false;}

    TechDraw::DrawViewClip* getViewObject() const override;
    TechDraw::DrawViewClip* getObject() const;

    void onChanged(const App::Property *prop) override;

    /// Hide the object in the view
    void hide() override;
    /// Show the object in the view
    void show() override;
    std::vector<App::DocumentObject*> claimChildren() const override;

    bool canDelete(App::DocumentObject* obj) const override;

    void dragObject(App::DocumentObject* docObj) override;
    void dropObject(App::DocumentObject* docObj) override;
};
} // namespace TechDrawGui

#endif // DRAWINGGUI_VIEWPROVIDERCLIP_H
