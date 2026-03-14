#include <QMenu>


#include <App/Document.h>
#include <Gui/Application.h>
#include <Mod/PartDesign/App/FeatureExtrude.h>
#include <Mod/Part/Gui/ReferenceHighlighter.h>

#include "TaskExtrudeParameters.h"
#include "ViewProviderExtrude.h"


using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderExtrude, PartDesignGui::ViewProviderSketchBased)

void PartDesignGui::ViewProviderExtrude::highlightShapeFaces(const std::vector<std::string>& faces)
{
    auto extrude = getObject<PartDesign::FeatureExtrude>();
    auto base = static_cast<Part::Feature*>(extrude->UpToShape.getValue());

    auto baseViewProvider = dynamic_cast<PartGui::ViewProviderPart*>(
        Gui::Application::Instance->getViewProvider(base)
    );

    if (!baseViewProvider) {
        return;
    }

    baseViewProvider->unsetHighlightedFaces();
    baseViewProvider->updateView();

    if (faces.size() > 0) {
        std::vector<App::Material> materials = baseViewProvider->ShapeAppearance.getValues();

        auto color = baseViewProvider->ShapeAppearance.getDiffuseColor();

        PartGui::ReferenceHighlighter highlighter(base->Shape.getValue(), color);
        highlighter.getFaceMaterials(faces, materials);

        baseViewProvider->setHighlightedFaces(materials);
    }
}
