#include <QMenu>

#include "ViewProviderBlendCurve.h"
#include "TaskBlendCurve.h"
#include <Gui/ActionFunction.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Control.h>

PROPERTY_SOURCE(SurfaceGui::ViewProviderBlendCurve, PartGui::ViewProviderSpline)

namespace SurfaceGui
{

QIcon ViewProviderBlendCurve::getIcon() const
{
    return Gui::BitmapFactory().pixmap("Surface_BlendCurve");
}

void ViewProviderBlendCurve::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    auto func = new Gui::ActionFunction(menu);
    QAction* act = menu->addAction(
        QObject::tr("Edit %1").arg(QString::fromUtf8(getObject()->Label.getValue()))
    );
    act->setData(QVariant((int)ViewProvider::Default));
    func->trigger(act, [this]() { this->startDefaultEditMode(); });

    ViewProviderSpline::setupContextMenu(menu, receiver, member);
}

bool ViewProviderBlendCurve::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        if (Gui::Control().activeDialog()) {
            return false;
        }
        auto dlg = new TaskBlendCurve(this);
        Gui::Control().showDialog(dlg);
        return true;
    }

    return ViewProviderSpline::setEdit(ModNum);
}

void ViewProviderBlendCurve::unsetEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
    }
    else {
        ViewProviderSpline::unsetEdit(ModNum);
    }
}

}  // namespace SurfaceGui
