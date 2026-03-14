#ifndef DRAWINGGUI_VIEWPROVIDERANNOTATION_H
#define DRAWINGGUI_VIEWPROVIDERANNOTATION_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <Mod/TechDraw/App/DrawViewAnnotation.h>

#include "ViewProviderDrawingView.h"


namespace TechDrawGui
{


class TechDrawGuiExport ViewProviderAnnotation: public ViewProviderDrawingView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderAnnotation);

public:
    /// constructor
    ViewProviderAnnotation();
    /// destructor
    ~ViewProviderAnnotation() override;

    bool useNewSelectionModel() const override { return false; }
    void updateData(const App::Property*) override;

    std::vector<App::DocumentObject*> claimChildren(void) const override;

    TechDraw::DrawViewAnnotation* getViewObject() const override;

    bool doubleClicked() override;
    bool setEdit(int ModNum) override;
};

}// namespace TechDrawGui


#endif// DRAWINGGUI_VIEWPROVIDERANNOTATION_H
