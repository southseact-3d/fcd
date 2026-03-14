# include <cmath>
# include <sstream>

# include <QGraphicsItem>
# include <QRectF>
# include <QString>

#include <Mod/TechDraw/App/DrawViewImage.h>

#include "QGIViewImage.h"
#include "QGCustomClip.h"
#include "QGCustomImage.h"
#include "Rez.h"
#include "ViewProviderImage.h"


using namespace TechDrawGui;

QGIViewImage::QGIViewImage()
{
    setHandlesChildEvents(false);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
    setCacheMode(QGraphicsItem::NoCache);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    m_cliparea = new QGCustomClip();
    addToGroup(m_cliparea);
    m_cliparea->setRect(0., 0., 5., 5.);
    m_cliparea->centerAt(0., 0.);

    m_imageItem = new QGCustomImage();
    m_imageItem->setTransformationMode(Qt::SmoothTransformation);
    m_cliparea->addToGroup(m_imageItem);
    m_imageItem->centerAt(0., 0.);
}

QGIViewImage::~QGIViewImage()
{
    // m_imageItem belongs to this group and will be deleted by Qt
}

void QGIViewImage::setViewImageFeature(TechDraw::DrawViewImage *obj)
{
    setViewFeature(static_cast<TechDraw::DrawView *>(obj));
}

void QGIViewImage::updateView(bool update)
{
    auto viewImage( dynamic_cast<TechDraw::DrawViewImage *>(getViewObject()) );
    if (!viewImage)
        return;

    if (update ||
        viewImage->isTouched() ||
        viewImage->Width.isTouched() ||
        viewImage->Height.isTouched() ||
        viewImage->ImageFile.isTouched()) {
        draw();
    }

    if (viewImage->Scale.isTouched()) {
        draw();
    }

    QGIView::updateView(update);
}

void QGIViewImage::draw()
{
    if (!isVisible()) {
        return;
    }

    auto viewImage( dynamic_cast<TechDraw::DrawViewImage*>(getViewObject()) );
    if (!viewImage)
        return;

    auto vp = static_cast<ViewProviderImage*>(getViewProvider(getViewObject()));
    if (!vp)
        return;
    bool crop = vp->Crop.getValue();

    drawImage();
    if (crop) {
        QRectF cropRect(0.0, 0.0, Rez::guiX(viewImage->Width.getValue()), Rez::guiX(viewImage->Height.getValue()));
        m_cliparea->setRect(cropRect);
    } else {
        QRectF cropRect(0.0, 0.0, m_imageItem->imageSize().width(), m_imageItem->imageSize().height());
        m_cliparea->setRect(cropRect);
    }
    m_cliparea->centerAt(0.0, 0.0);

    drawImage();
}

void QGIViewImage::drawImage()
{
    auto viewImage( dynamic_cast<TechDraw::DrawViewImage *>(getViewObject()) );
    if (!viewImage)
        return;

    if (!viewImage->ImageIncluded.isEmpty()) {
        QString fileSpec = QString::fromUtf8(viewImage->ImageIncluded.getValue(), strlen(viewImage->ImageIncluded.getValue()));
        m_imageItem->load(fileSpec);
        m_imageItem->setScale(viewImage->getScale());
        QRectF br = m_cliparea->rect();
        double midX = br.width()/2.0;
        double midY = br.height()/2.0;
        m_imageItem->centerAt(midX, midY);
        m_imageItem->show();
    }
}

void QGIViewImage::rotateView()
{
    QRectF r = m_cliparea->boundingRect();
    m_cliparea->setTransformOriginPoint(r.center());
    double rot = getViewObject()->Rotation.getValue();
    m_cliparea->setRotation(-rot);
}
