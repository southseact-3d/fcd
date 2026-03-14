#include <QMenu>
#include <QMessageBox>


#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/Control.h>
#include <Gui/MainWindow.h>
#include <Gui/TaskView/TaskDialog.h>
#include <Mod/PartDesign/App/FeaturePrimitive.h>

#include "ViewProviderPrimitive.h"
#include "TaskPrimitiveParameters.h"


using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderPrimitive, PartDesignGui::ViewProvider)

ViewProviderPrimitive::ViewProviderPrimitive() = default;

ViewProviderPrimitive::~ViewProviderPrimitive() = default;

void ViewProviderPrimitive::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Primitive"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

TaskDlgFeatureParameters* ViewProviderPrimitive::getEditDialog()
{
    return new TaskDlgPrimitiveParameters(this);
}

QIcon ViewProviderPrimitive::getIcon() const
{

    QString str = QStringLiteral("PartDesign_");
    auto* prim = getObject<PartDesign::FeaturePrimitive>();
    if (prim->getAddSubType() == PartDesign::FeatureAddSub::Additive) {
        str += QStringLiteral("Additive");
    }
    else {
        str += QStringLiteral("Subtractive");
    }

    switch (prim->getPrimitiveType()) {
        case PartDesign::FeaturePrimitive::Box:
            str += QStringLiteral("Box");
            break;
        case PartDesign::FeaturePrimitive::Cylinder:
            str += QStringLiteral("Cylinder");
            break;
        case PartDesign::FeaturePrimitive::Sphere:
            str += QStringLiteral("Sphere");
            break;
        case PartDesign::FeaturePrimitive::Cone:
            str += QStringLiteral("Cone");
            break;
        case PartDesign::FeaturePrimitive::Ellipsoid:
            str += QStringLiteral("Ellipsoid");
            break;
        case PartDesign::FeaturePrimitive::Torus:
            str += QStringLiteral("Torus");
            break;
        case PartDesign::FeaturePrimitive::Prism:
            str += QStringLiteral("Prism");
            break;
        case PartDesign::FeaturePrimitive::Wedge:
            str += QStringLiteral("Wedge");
            break;
    }

    str += QStringLiteral(".svg");
    return PartDesignGui::ViewProvider::mergeGreyableOverlayIcons(
        Gui::BitmapFactory().pixmap(str.toStdString().c_str())
    );
}
