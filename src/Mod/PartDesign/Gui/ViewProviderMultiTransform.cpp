#include "ViewProviderMultiTransform.h"
#include "TaskMultiTransformParameters.h"
#include <Mod/PartDesign/App/FeatureMultiTransform.h>
#include <App/Document.h>
#include <Gui/Command.h>

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderMultiTransform, PartDesignGui::ViewProviderTransformed)

TaskDlgFeatureParameters* ViewProviderMultiTransform::getEditDialog()
{
    return new TaskDlgMultiTransformParameters(this);
}

const std::string& ViewProviderMultiTransform::featureName() const
{
    static const std::string name = "MultiTransform";
    return name;
}

void ViewProviderMultiTransform::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Multi-Transform"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);  // clazy:exclude=skipped-base-method
}

std::vector<App::DocumentObject*> ViewProviderMultiTransform::claimChildren() const
{
    PartDesign::MultiTransform* pcMultiTransform = getObject<PartDesign::MultiTransform>();
    if (!pcMultiTransform) {
        return {};  // TODO: Show error?
    }

    std::vector<App::DocumentObject*> transformFeatures = pcMultiTransform->Transformations.getValues();
    return transformFeatures;
}

bool ViewProviderMultiTransform::onDelete(const std::vector<std::string>& svec)
{
    // Delete the transformation features
    PartDesign::MultiTransform* pcMultiTransform = getObject<PartDesign::MultiTransform>();
    std::vector<App::DocumentObject*> transformFeatures = pcMultiTransform->Transformations.getValues();

    // if the multitransform object was deleted the transformed features must be deleted, too
    for (auto it : transformFeatures) {
        if (it) {
            Gui::Command::doCommand(
                Gui::Command::Doc,
                "App.getDocument('%s').removeObject(\"%s\")",
                it->getDocument()->getName(),
                it->getNameInDocument()
            );
        }
    }

    // Handle Originals
    return ViewProviderTransformed::onDelete(svec);
}
