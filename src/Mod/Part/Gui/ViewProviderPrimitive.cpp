#include <QAction>
#include <QMenu>


#include <Gui/ActionFunction.h>
#include <Gui/Control.h>
#include <Mod/Part/App/PrimitiveFeature.h>

#include "ViewProviderPrimitive.h"
#include "DlgPrimitives.h"


using namespace PartGui;

PROPERTY_SOURCE(PartGui::ViewProviderPrimitive, PartGui::ViewProviderPart)

ViewProviderPrimitive::ViewProviderPrimitive()
{
    extension.initExtension(this);
    extension.setIgnoreOverlayIcon(true);
}

ViewProviderPrimitive::~ViewProviderPrimitive() = default;

void ViewProviderPrimitive::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    Gui::ActionFunction* func = new Gui::ActionFunction(menu);
    QAction* act = menu->addAction(
        QObject::tr("Edit %1").arg(QString::fromUtf8(getObject()->Label.getValue()))
    );
    act->setData(QVariant((int)ViewProvider::Default));
    func->trigger(act, [this]() { this->startDefaultEditMode(); });

    ViewProviderPart::setupContextMenu(menu, receiver, member);
}

bool ViewProviderPrimitive::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        if (Gui::Control().activeDialog()) {
            return false;
        }
        PartGui::TaskPrimitivesEdit* dlg = new PartGui::TaskPrimitivesEdit(
            getObject<Part::Primitive>()
        );
        Gui::Control().showDialog(dlg);
        return true;
    }
    else {
        ViewProviderPart::setEdit(ModNum);
        return true;
    }
}

void ViewProviderPrimitive::unsetEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
    }
    else {
        ViewProviderPart::unsetEdit(ModNum);
    }
}
