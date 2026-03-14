#include "TaskFilletParameters.h"
#include "ViewProviderFillet.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderFillet, PartDesignGui::ViewProviderDressUp)


const std::string& ViewProviderFillet::featureName() const
{
    static const std::string name = "Fillet";
    return name;
}

void ViewProviderFillet::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Fillet"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

TaskDlgFeatureParameters* ViewProviderFillet::getEditDialog()
{
    return new TaskDlgFilletParameters(this);
}
