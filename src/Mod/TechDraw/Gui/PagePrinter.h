#ifndef TECHDRAWGUI_PAGEPRINTER_H
#define TECHDRAWGUI_PAGEPRINTER_H

#include <QPrinter>


#include <Mod/TechDraw/TechDrawGlobal.h>

#include "ViewProviderPage.h"

QT_BEGIN_NAMESPACE
class QGraphicsScene;
QT_END_NAMESPACE

namespace TechDraw {
class DrawPage;
class DrawTemplate;
class DrawView;
}

namespace TechDrawGui
{
class MDIViewPage;
class QGVPage;
class QGSPage;
class QGIView;

class TechDrawGuiExport PaperAttributes
{
public:
    PaperAttributes();
    PaperAttributes(QPageLayout::Orientation orientation,
                    QPageSize::PageSizeId paperSizeId,
                    double pageWidth,
                    double pageHeight)
        : m_orientation(orientation)
        , m_paperSizeId(paperSizeId)
        , m_pagewidth(pageWidth)
        , m_pageheight(pageHeight)
    {}

    QPageLayout::Orientation orientation() const
    {
        return m_orientation;
    }
    QPageSize::PageSizeId pageSizeId() const
    {
        return m_paperSizeId;
    }
    double pageWidth() const
    {
        return m_pagewidth;
    }
    double pageHeight() const
    {
        return m_pageheight;
    }

private:
    QPageLayout::Orientation m_orientation;
    QPageSize::PageSizeId m_paperSizeId;
    double m_pagewidth;
    double m_pageheight;
};

class TechDrawGuiExport PagePrinter
{
public:
    // print banner page is no longer used
    static void printBannerPage(QPrinter* printer, QPainter& painter,
                                QPageLayout& pageLayout,
                                App::Document* doc,
                                std::vector<App::DocumentObject*>& docObjs);

    static void renderPage(ViewProviderPage* vpp,
                           QPainter& painter,
                           QRectF& sourceRect,
                           QRect& targetRect);
    static void makePageLayout(TechDraw::DrawPage* dPage,
                               QPageLayout& pageLayout,
                              double& width, double& height);

    static PaperAttributes getPaperAttributes(TechDraw::DrawPage* pageObject);
    static PaperAttributes getPaperAttributes(ViewProviderPage* vpPage);

    static void print(ViewProviderPage* vpPage, QPrinter* printer, bool isPreview = false);
    static void printPdf(ViewProviderPage* vpPage, const std::string& file);
    static void printAll(QPrinter* printer, App::Document* doc);
    static void printAllPdf(QPrinter* printer, App::Document* doc);

    static void saveSVG(ViewProviderPage* vpPage, const std::string& file);
    static void saveDXF(ViewProviderPage* vpPage, const std::string& file);
    static void savePDF(ViewProviderPage* vpPage, const std::string& file);
};

}  // namespace TechDrawGui

#endif // TECHDRAWGUI_PAGEPRINTER_H

