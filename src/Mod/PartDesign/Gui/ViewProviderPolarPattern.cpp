#include "ViewProviderPolarPattern.h"
#include "TaskPatternParameters.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderPolarPattern, PartDesignGui::ViewProviderTransformed)

TaskDlgFeatureParameters* ViewProviderPolarPattern::getEditDialog()
{
    return new TaskDlgLinearPatternParameters(this);
}

void ViewProviderPolarPattern::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Polar Pattern"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

const std::string& ViewProviderPolarPattern::featureName() const
{
    static const std::string name = "PolarPattern";
    return name;
}
