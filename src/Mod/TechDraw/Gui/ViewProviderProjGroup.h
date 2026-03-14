#ifndef DRAWINGGUI_VIEWPROVIDERVIEWGROUP_H
#define DRAWINGGUI_VIEWPROVIDERVIEWGROUP_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <Mod/TechDraw/App/DrawProjGroup.h>

#include "ViewProviderDrawingView.h"


namespace TechDrawGui {

class TechDrawGuiExport ViewProviderProjGroup : public ViewProviderDrawingView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderProjGroup);

public:

     ViewProviderProjGroup();  /// constructor
     ~ViewProviderProjGroup() override = default; /// destructor

    bool useNewSelectionModel() const override {return false;}

    /// Claim all the views for the group
    std::vector<App::DocumentObject*> claimChildren() const override;

    /// Is called by the tree if the user double click on the object
    bool doubleClicked() override;

    TechDraw::DrawProjGroup* getObject() const;
    TechDraw::DrawProjGroup* getViewObject() const override;
    bool onDelete(const std::vector<std::string> &parms) override;
    bool canDelete(App::DocumentObject* obj) const override;

    void regroupSubViews();
    void updateData(const App::Property* prop) override;
    void onChangeAutoDistribute();


protected:
    bool setEdit(int ModNum) override;

};

} // namespace TechDrawGui

#endif // DRAWINGGUI_VIEWPROVIDERVIEWGROUP_H
