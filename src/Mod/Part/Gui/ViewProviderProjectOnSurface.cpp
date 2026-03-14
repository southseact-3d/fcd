#include <QAction>
#include <QMenu>


#include "ViewProviderProjectOnSurface.h"
#include "DlgProjectionOnSurface.h"
#include <Gui/Control.h>


using namespace PartGui;

PROPERTY_SOURCE(PartGui::ViewProviderProjectOnSurface, PartGui::ViewProviderPart)


ViewProviderProjectOnSurface::ViewProviderProjectOnSurface()
{
    const unsigned int color = 0x8ae23400;
    LineColor.setValue(color);
    ShapeAppearance.setDiffuseColor(color);
    PointColor.setValue(color);
    Transparency.setValue(0);
}

ViewProviderProjectOnSurface::~ViewProviderProjectOnSurface() = default;

void ViewProviderProjectOnSurface::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    QAction* act = menu->addAction(QObject::tr("Edit Projection"), receiver, member);
    act->setData(QVariant((int)ViewProvider::Default));

    ViewProviderPart::setupContextMenu(menu, receiver, member);
}

bool ViewProviderProjectOnSurface::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        if (Gui::Control().activeDialog()) {
            return false;
        }

        if (auto feature = getObject<Part::ProjectOnSurface>()) {
            Gui::Control().showDialog(new TaskProjectOnSurface(feature));
            return true;
        }

        return false;
    }

    return ViewProviderPart::setEdit(ModNum);
}

void ViewProviderProjectOnSurface::unsetEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
    }
    else {
        ViewProviderPart::unsetEdit(ModNum);
    }
}
