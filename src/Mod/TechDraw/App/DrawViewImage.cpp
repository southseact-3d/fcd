# include <iomanip>
# include <sstream>


#include <App/Document.h>
#include <Base/Console.h>
#include <Base/FileInfo.h>

#include "DrawUtil.h"
#include "DrawViewImage.h"


using namespace TechDraw;

//===========================================================================
// DrawViewImage
//===========================================================================

PROPERTY_SOURCE(TechDraw::DrawViewImage, TechDraw::DrawView)


DrawViewImage::DrawViewImage()
{
    static const char* vgroup = "Image";

    ADD_PROPERTY_TYPE(ImageFile, (""), vgroup, App::Prop_None, "The file containing this bitmap");
    ADD_PROPERTY_TYPE(ImageIncluded, (""), vgroup, App::Prop_None,
                      "Embedded image file. System use only.");// n/a to end users
    ADD_PROPERTY_TYPE(Width, (100), vgroup, App::Prop_None, "The width of cropped image");
    ADD_PROPERTY_TYPE(Height, (100), vgroup, App::Prop_None, "The height of cropped image");
    ADD_PROPERTY_TYPE(Owner, (nullptr), vgroup, (App::PropertyType)(App::Prop_None),
                      "Feature to which this symbol is attached");

    ScaleType.setValue("Custom");
    Scale.setStatus(App::Property::Hidden, false);
    Scale.setStatus(App::Property::ReadOnly, false);

    std::string imgFilter("Image files (*.jpg *.jpeg *.png *.bmp);;All files (*)");
    ImageFile.setFilter(imgFilter);
}

void DrawViewImage::onChanged(const App::Property* prop)
{
    if (isRestoring()) {
        TechDraw::DrawView::onChanged(prop);
        return;
    }

    if (prop == &ImageFile) {
        replaceImageIncluded(ImageFile.getValue());
        requestPaint();
    }

    TechDraw::DrawView::onChanged(prop);
}

App::DocumentObjectExecReturn* DrawViewImage::execute()
{
    requestPaint();
    return DrawView::execute();
}

QRectF DrawViewImage::getRect() const { return {0.0, 0.0, Width.getValue(), Height.getValue()}; }

void DrawViewImage::replaceImageIncluded(std::string newImageFile)
{
    //    Base::Console().message("DVI::replaceImageIncluded(%s)\n", newImageFile.c_str());
    if (newImageFile.empty()) {
        return;
    }

    Base::FileInfo tfi(newImageFile);
    if (tfi.isReadable()) {
        ImageIncluded.setValue(newImageFile.c_str());
    }
    else {
        throw Base::RuntimeError("Could not read the new image file");
    }
}

void DrawViewImage::setupObject()
{
    //    Base::Console().message("DVI::setupObject()\n");
    replaceImageIncluded(ImageFile.getValue());
}

// Python Drawing feature ---------------------------------------------------------

namespace App
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(TechDraw::DrawViewImagePython, TechDraw::DrawViewImage)
template<> const char* TechDraw::DrawViewImagePython::getViewProviderName() const
{
    return "TechDrawGui::ViewProviderImage";
}
/// @endcond

// explicit template instantiation
template class TechDrawExport FeaturePythonT<TechDraw::DrawViewImage>;
}// namespace App
