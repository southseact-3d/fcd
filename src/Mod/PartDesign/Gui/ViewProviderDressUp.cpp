#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>


#include <Gui/Application.h>
#include <Mod/Part/Gui/ReferenceHighlighter.h>
#include <Mod/PartDesign/App/FeatureDressUp.h>

#include "ViewProviderDressUp.h"

#include "StyleParameters.h"
#include "TaskDressUpParameters.h"

#include <Base/ServiceProvider.h>
#include <Gui/Utilities.h>

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderDressUp, PartDesignGui::ViewProvider)


void ViewProviderDressUp::attach(App::DocumentObject* pcObject)
{
    ViewProvider::attach(pcObject);

    auto* styleParameterManager = Base::provideService<Gui::StyleParameters::ParameterManager>();
    PreviewColor.setValue(styleParameterManager->resolve(StyleParameters::PreviewDressUpColor));

    setErrorState(false);
}

void ViewProviderDressUp::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    QString text = QString::fromStdString(getObject()->Label.getStrValue());
    addDefaultAction(menu, QObject::tr("Edit %1").arg(text));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

const std::string& ViewProviderDressUp::featureName() const
{
    static const std::string name = "Undefined";
    return name;
}

std::string ViewProviderDressUp::featureIcon() const
{
    return std::string("PartDesign_") + featureName();
}


bool ViewProviderDressUp::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        // Here we should prevent edit of a Feature with missing base
        // Otherwise it could call unhandled exception.
        PartDesign::DressUp* dressUp = getObject<PartDesign::DressUp>();
        assert(dressUp);
        if (dressUp->getBaseObject(/*silent =*/true)) {
            return ViewProvider::setEdit(ModNum);
        }
        else {
            QMessageBox::warning(
                nullptr,
                QObject::tr("Feature error"),
                QObject::tr(
                    "%1 misses a base feature.\n"
                    "This feature is broken and cannot be edited."
                )
                    .arg(QString::fromLatin1(dressUp->getNameInDocument()))
            );
            return false;
        }
    }
    else {
        return ViewProvider::setEdit(ModNum);
    }
}

void ViewProviderDressUp::highlightReferences(const bool on)
{
    PartDesign::DressUp* pcDressUp = getObject<PartDesign::DressUp>();
    Part::Feature* base = pcDressUp->getBaseObject(/*silent =*/true);
    if (!base) {
        return;
    }
    PartGui::ViewProviderPart* vp = dynamic_cast<PartGui::ViewProviderPart*>(
        Gui::Application::Instance->getViewProvider(base)
    );
    if (!vp) {
        return;
    }

    std::vector<std::string> faces = pcDressUp->Base.getSubValuesStartsWith("Face");
    std::vector<std::string> edges = pcDressUp->Base.getSubValuesStartsWith("Edge");

    if (on) {
        if (!faces.empty()) {
            std::vector<App::Material> materials = vp->ShapeAppearance.getValues();

            PartGui::ReferenceHighlighter highlighter(
                base->Shape.getValue(),
                ShapeAppearance.getDiffuseColor()
            );
            highlighter.getFaceMaterials(faces, materials);

            vp->setHighlightedFaces(materials);
        }
        if (!edges.empty()) {
            std::vector<Base::Color> colors = vp->LineColorArray.getValues();

            PartGui::ReferenceHighlighter highlighter(base->Shape.getValue(), LineColor.getValue());
            highlighter.getEdgeColors(edges, colors);

            vp->setHighlightedEdges(colors);
        }
    }
    else {
        vp->unsetHighlightedFaces();
        vp->unsetHighlightedEdges();
    }
}

void ViewProviderDressUp::setErrorState(bool error)
{
    auto* styleParameterManager = Base::provideService<Gui::StyleParameters::ParameterManager>();

    const float opacity = static_cast<float>(
        styleParameterManager
            ->resolve(error ? StyleParameters::PreviewErrorOpacity : StyleParameters::PreviewShapeOpacity)
            .value
    );

    pcPreviewShape->transparency = 1.0F - opacity;
    pcPreviewShape->color = error
        ? styleParameterManager->resolve(StyleParameters::PreviewErrorColor).asValue<SbColor>()
        : PreviewColor.getValue().asValue<SbColor>();
}
