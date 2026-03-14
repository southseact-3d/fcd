#include "ViewProviderScaled.h"
#include "TaskScaledParameters.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderScaled, PartDesignGui::ViewProviderTransformed)

TaskDlgFeatureParameters* ViewProviderScaled::getEditDialog()
{
    return new TaskDlgScaledParameters(this);
}

void ViewProviderScaled::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Scale"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

const std::string& ViewProviderScaled::featureName() const
{
    static const std::string name = "Scaled";
    return name;
}
