#include "TaskDraftParameters.h"
#include "ViewProviderDraft.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderDraft, PartDesignGui::ViewProviderDressUp)


const std::string& ViewProviderDraft::featureName() const
{
    static const std::string name = "Draft";
    return name;
}

void ViewProviderDraft::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Draft"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

TaskDlgFeatureParameters* ViewProviderDraft::getEditDialog()
{
    return new TaskDlgDraftParameters(this);
}
