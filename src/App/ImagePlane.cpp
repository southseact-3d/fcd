#include "ImagePlane.h"


using namespace Image;

PROPERTY_SOURCE(Image::ImagePlane, App::GeoFeature)


ImagePlane::ImagePlane()
{
    ADD_PROPERTY_TYPE(ImageFile, (nullptr), "ImagePlane", App::Prop_None, "File of the image");
    ADD_PROPERTY_TYPE(XSize, (100), "ImagePlane", App::Prop_None, "Size of a pixel in X");
    ADD_PROPERTY_TYPE(YSize, (100), "ImagePlane", App::Prop_None, "Size of a pixel in Y");
}

int ImagePlane::getXSizeInPixel()
{
    return int(XSize.getValue() * XPixelsPerMeter / 1000);
}

int ImagePlane::getYSizeInPixel()
{
    return int(YSize.getValue() * YPixelsPerMeter / 1000);
}

void ImagePlane::setXSizeInPixel(int value)
{
    XSize.setValue(double(value) * 1000.0 / XPixelsPerMeter);
}

void ImagePlane::setYSizeInPixel(int value)
{
    YSize.setValue(double(value) * 1000.0 / YPixelsPerMeter);
}
