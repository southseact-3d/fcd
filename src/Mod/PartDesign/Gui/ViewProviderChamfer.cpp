#include "TaskChamferParameters.h"
#include "ViewProviderChamfer.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderChamfer, PartDesignGui::ViewProviderDressUp)


const std::string& ViewProviderChamfer::featureName() const
{
    static const std::string name = "Chamfer";
    return name;
}

void ViewProviderChamfer::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Chamfer"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

TaskDlgFeatureParameters* ViewProviderChamfer::getEditDialog()
{
    return new TaskDlgChamferParameters(this);
}
