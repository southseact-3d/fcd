#ifndef DRAWINGGUI_VIEWPROVIDERIMAGE_H
#define DRAWINGGUI_VIEWPROVIDERIMAGE_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <Mod/TechDraw/App/DrawViewImage.h>

#include "ViewProviderDrawingView.h"


namespace TechDrawGui {


class TechDrawGuiExport ViewProviderImage : public ViewProviderDrawingView
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderImage);

public:
    /// constructor
    ViewProviderImage();
    /// destructor
    ~ViewProviderImage() override;

    App::PropertyBool  Crop;              //crop to feature width x height

    bool useNewSelectionModel() const override {return false;}
    /// returns a list of all possible modes
    void updateData(const App::Property*) override;
    void onChanged(const App::Property *prop) override;

    TechDraw::DrawViewImage* getViewObject() const override;
};


} // namespace TechDrawGui


#endif // DRAWINGGUI_VIEWPROVIDERIMAGE_H
