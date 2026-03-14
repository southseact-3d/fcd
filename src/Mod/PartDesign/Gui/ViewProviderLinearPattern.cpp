#include "ViewProviderLinearPattern.h"
#include "TaskPatternParameters.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderLinearPattern, PartDesignGui::ViewProviderTransformed)

TaskDlgFeatureParameters* ViewProviderLinearPattern::getEditDialog()
{
    return new TaskDlgLinearPatternParameters(this);
}

void ViewProviderLinearPattern::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Linear Pattern"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

const std::string& ViewProviderLinearPattern::featureName() const
{
    static const std::string name = "LinearPattern";
    return name;
}
