#include <Inventor/SbImage.h>
#include <QImage>

#include "ImageReader.h"
#include "QtCoinCompatibility.h"


using namespace SIM::Coin3D::Quarter;

ImageReader::ImageReader()
{
  SbImage::addReadImageCB(ImageReader::readImageCB, this);
}

ImageReader::~ImageReader()
{
  SbImage::removeReadImageCB(ImageReader::readImageCB, this);
}

SbBool
ImageReader::readImage(const SbString & filename, SbImage & sbimage) const
{
  QImage image;
  if (image.load(filename.getString())) {
    // Keep in 8-bits mode if that was what we read
    if (image.depth() != 8 || !image.isGrayscale()) {
      // FIXME: consider if we should detect allGrayscale() and alpha (c = 2)
      image = image.convertToFormat(image.hasAlphaChannel() ?
                                    QImage::Format_ARGB32 : QImage::Format_RGB32);
    }

    QtCoinCompatibility::QImageToSbImage(image,sbimage);
    return true;
  }
  return false;
}



SbBool
ImageReader::readImageCB(const SbString & filename, SbImage * image, void * closure)
{
  return ((ImageReader*)closure)->readImage(filename, *image);
}
