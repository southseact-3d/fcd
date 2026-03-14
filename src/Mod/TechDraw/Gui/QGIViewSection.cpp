# include <cmath>


#include <Mod/TechDraw/App/DrawViewSection.h>

#include "QGIViewSection.h"
#include "QGIFace.h"
#include "ViewProviderViewSection.h"
#include "ZVALUE.h"


using namespace TechDrawGui;
using FillMode = QGIFace::FillMode;

void QGIViewSection::draw()
{
    if (!isVisible()) {
        return;
    }

    QGIViewPart::draw();
    drawSectionFace();
}

void QGIViewSection::drawSectionFace()
{
    auto section( dynamic_cast<TechDraw::DrawViewSection *>(getViewObject()) );
    if (!section) {
        return;
    }

    if (!section->hasGeometry()) {
        return;
    }

    ViewProviderViewSection* sectionVp = freecad_cast<ViewProviderViewSection*>(QGIView::getViewProvider(section));
    if (!sectionVp) {
        return;
    }

    auto sectionFaces( section->getTDFaceGeometry() );
    if (sectionFaces.empty()) {
        return;
    }

    float lineWidth    = sectionVp->LineWidth.getValue();

    std::vector<TechDraw::FacePtr>::iterator fit = sectionFaces.begin();
    int i = 0;
    for(; fit != sectionFaces.end(); fit++, i++) {
        QGIFace* newFace = drawFace(*fit, -1);
        newFace->setZValue(ZVALUE::SECTIONFACE);
        if (section->showSectionEdges()) {
            newFace->setDrawEdges(true);
            newFace->setStyle(Qt::SolidLine);
            newFace->setWidth(lineWidth);
        } else {
            newFace->setDrawEdges(false);
        }

        if (section->CutSurfaceDisplay.isValue("Hide")) {
            return;
        }

        if (section->CutSurfaceDisplay.isValue("Color")) {
            newFace->isHatched(true);
            QColor faceColor = (sectionVp->CutSurfaceColor.getValue()).asValue<QColor>();
            faceColor.setAlpha((100 - sectionVp->CutSurfaceTransparency.getValue())*255/100);
            newFace->setFillColor(faceColor);
            newFace->setFillMode(faceColor.alpha() ? FillMode::PlainFill : FillMode::NoFill);
        } else if (section->CutSurfaceDisplay.isValue("SvgHatch")) {
            newFace->isHatched(true);
            newFace->setFillMode(FillMode::SvgFill);
            newFace->setHatchColor(sectionVp->HatchColor.getValue());
            newFace->setHatchScale(section->HatchScale.getValue());
            newFace->setHatchRotation(section->HatchRotation.getValue());
            newFace->setHatchOffset(section->HatchOffset.getValue());
            std::string hatchSpec = section->SvgIncluded.getValue();
            newFace->setHatchFile(hatchSpec);
        } else if (section->CutSurfaceDisplay.isValue("PatHatch")) {
            newFace->isHatched(true);
            newFace->setFillMode(FillMode::GeomHatchFill);
            newFace->setHatchColor(sectionVp->GeomHatchColor.getValue());
            newFace->setHatchScale(section->HatchScale.getValue());
            newFace->setHatchRotation(section->HatchRotation.getValue());
            newFace->setHatchOffset(section->HatchOffset.getValue());
            newFace->setLineWeight(sectionVp->WeightPattern.getValue());
            std::vector<TechDraw::LineSet> lineSets = section->getDrawableLines(i);
            if (!lineSets.empty()) {
                for (auto& ls: lineSets) {
                    newFace->addLineSet(ls);
                }
            }
        } else {
            Base::Console().warning("QGIVS::draw - unknown CutSurfaceDisplay: %d\n",
                                    section->CutSurfaceDisplay.getValue());
        }

        newFace->draw();
        newFace->setPrettyNormal();
        newFace->setAcceptHoverEvents(false);
        newFace->setFlag(QGraphicsItem::ItemIsSelectable, false);
    }
}

void QGIViewSection::updateView(bool update)
{
    Q_UNUSED(update);
    auto viewPart( dynamic_cast<TechDraw::DrawViewSection *>(getViewObject()) );
    if (!viewPart)
        return;
    draw();
    QGIView::updateView(update);
}
