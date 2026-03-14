#ifndef App_ImagePlane_H
#define App_ImagePlane_H

#include <App/GeoFeature.h>
#include <App/PropertyFile.h>
#include <App/PropertyUnits.h>

namespace Image
{

class AppExport ImagePlane: public App::GeoFeature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Image::ImagePlane);

public:
    /// Constructor
    ImagePlane();
    ~ImagePlane() override = default;

    App::PropertyFileIncluded ImageFile;
    App::PropertyLength XSize;
    App::PropertyLength YSize;

    int getXSizeInPixel();
    int getYSizeInPixel();
    void setXSizeInPixel(int);
    void setYSizeInPixel(int);

    double XPixelsPerMeter {1000.0};
    double YPixelsPerMeter {1000.0};

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "Gui::ViewProviderImagePlane";
    }
};

}  // namespace Image


#endif  // App_ImagePlane_H
