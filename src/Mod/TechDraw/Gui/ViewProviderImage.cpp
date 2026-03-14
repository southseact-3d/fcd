#include <App/DocumentObject.h>

#include "ViewProviderImage.h"
#include "QGIView.h"

using namespace TechDrawGui;

PROPERTY_SOURCE(TechDrawGui::ViewProviderImage, TechDrawGui::ViewProviderDrawingView)

//**************************************************************************
// Construction/Destruction

ViewProviderImage::ViewProviderImage()
{
    sPixmap = "actions/TechDraw_Image";

    ADD_PROPERTY_TYPE(Crop ,(false), "Image", App::Prop_None, "Crop image to Width x Height");
}

ViewProviderImage::~ViewProviderImage()
{
}

void ViewProviderImage::updateData(const App::Property* prop)
{
    if (prop == &(getViewObject()->Width)  ||
        prop == &(getViewObject()->Height)  ||
        prop == &(getViewObject()->Scale) ) {
        QGIView* qgiv = getQView();
        if (qgiv) {
            qgiv->QGIView::updateView(true);
        }
    }

    ViewProviderDrawingView::updateData(prop);
}

void ViewProviderImage::onChanged(const App::Property *prop)
{
    App::DocumentObject* obj = getObject();
    if (!obj || obj->isRestoring()) {
            Gui::ViewProviderDocumentObject::onChanged(prop);
            return;
    }

    if (prop == &Crop) {
        QGIView* qgiv = getQView();
        if (qgiv) {
            qgiv->updateView(true);
        }
    }

    Gui::ViewProviderDocumentObject::onChanged(prop);
}


TechDraw::DrawViewImage* ViewProviderImage::getViewObject() const
{
    return dynamic_cast<TechDraw::DrawViewImage*>(pcObject);
}


