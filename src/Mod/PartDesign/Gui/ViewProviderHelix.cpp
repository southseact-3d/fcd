#include <QAction>
#include <QMenu>


#include <Mod/PartDesign/App/FeatureHelix.h>
#include <Gui/BitmapFactory.h>

#include <Gui/Application.h>
#include <Mod/Sketcher/App/SketchObject.h>

#include "TaskHelixParameters.h"
#include "ViewProviderHelix.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderHelix, PartDesignGui::ViewProvider)


ViewProviderHelix::ViewProviderHelix() = default;

ViewProviderHelix::~ViewProviderHelix() = default;

void ViewProviderHelix::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Helix"));
    ViewProvider::setupContextMenu(menu, receiver, member);
}

TaskDlgFeatureParameters* ViewProviderHelix::getEditDialog()
{
    return new TaskDlgHelixParameters(this);
}

QIcon ViewProviderHelix::getIcon() const
{
    QString str = QStringLiteral("PartDesign_");
    auto* prim = getObject<PartDesign::Helix>();
    if (prim->getAddSubType() == PartDesign::FeatureAddSub::Additive) {
        str += QStringLiteral("Additive");
    }
    else {
        str += QStringLiteral("Subtractive");
    }

    str += QStringLiteral("Helix.svg");
    return PartDesignGui::ViewProvider::mergeGreyableOverlayIcons(
        Gui::BitmapFactory().pixmap(str.toStdString().c_str())
    );
}

std::vector<App::DocumentObject*> ViewProviderHelix::claimChildren() const
{
    std::vector<App::DocumentObject*> temp;
    App::DocumentObject* sketch = getObject<PartDesign::ProfileBased>()->Profile.getValue();
    if (sketch && sketch->isDerivedFrom<Part::Part2DObject>()) {
        temp.push_back(sketch);
    }

    return temp;
}
