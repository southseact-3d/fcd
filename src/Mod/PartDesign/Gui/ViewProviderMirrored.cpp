#include "ViewProviderMirrored.h"
#include "TaskMirroredParameters.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderMirrored, PartDesignGui::ViewProviderTransformed)

TaskDlgFeatureParameters* ViewProviderMirrored::getEditDialog()
{
    return new TaskDlgMirroredParameters(this);
}

void ViewProviderMirrored::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Mirror"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

const std::string& ViewProviderMirrored::featureName() const
{
    static const std::string name = "Mirrored";
    return name;
}
