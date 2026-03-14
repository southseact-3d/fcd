#include "TaskThicknessParameters.h"
#include "ViewProviderThickness.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderThickness, PartDesignGui::ViewProviderDressUp)


const std::string& ViewProviderThickness::featureName() const
{
    static const std::string name = "Thickness";
    return name;
}

void ViewProviderThickness::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Thickness"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

TaskDlgFeatureParameters* ViewProviderThickness::getEditDialog()
{
    return new TaskDlgThicknessParameters(this);
}
