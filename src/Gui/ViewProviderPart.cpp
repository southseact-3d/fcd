#include <QMenu>


#include <App/Document.h>
#include <App/DocumentObject.h>
#include <App/Part.h>

#include "ViewProviderPart.h"
#include "ActionFunction.h"
#include "Application.h"
#include "BitmapFactory.h"
#include "Command.h"
#include "MDIView.h"


using namespace Gui;


PROPERTY_SOURCE_WITH_EXTENSIONS(Gui::ViewProviderPart, Gui::ViewProviderGeometryObject)


/**
 * Creates the view provider for an object group.
 */
ViewProviderPart::ViewProviderPart()
{
    initExtension(this);

    sPixmap = "Geofeaturegroup.svg";
    aPixmap = "Geoassembly.svg";
}

ViewProviderPart::~ViewProviderPart() = default;

/**
 * TODO
 * Whenever a property of the group gets changed then the same property of all
 * associated view providers of the objects of the object group get changed as well.
 */
void ViewProviderPart::onChanged(const App::Property* prop)
{
    ViewProviderGeometryObject::onChanged(prop);
}

void ViewProviderPart::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    auto func = new Gui::ActionFunction(menu);

    QAction* act = menu->addAction(QObject::tr("Active Object"));
    act->setCheckable(true);
    act->setChecked(isActivePart());
    func->trigger(act, [this]() { this->toggleActivePart(); });

    ViewProviderGeometryObject::setupContextMenu(menu, receiver, member);
}

bool ViewProviderPart::isActivePart(const char* key)
{
    App::DocumentObject* activePart = nullptr;
    auto activeDoc = Gui::Application::Instance->activeDocument();
    if (!activeDoc) {
        activeDoc = getDocument();
    }
    auto activeView = activeDoc->setActiveView(this);
    if (!activeView) {
        return false;
    }

    activePart = activeView->getActiveObject<App::DocumentObject*>(key);

    if (activePart == this->getObject()) {
        return true;
    }
    else {
        return false;
    }
}

void ViewProviderPart::toggleActivePart()
{
    // make the part the active one
    if (isActivePart()) {
        // active part double-clicked. Deactivate.
        Gui::Command::doCommand(
            Gui::Command::Gui,
            "Gui.ActiveDocument.ActiveView.setActiveObject('%s', None)",
            PARTKEY
        );
    }
    else {
        // set new active part
        Gui::Command::doCommand(
            Gui::Command::Gui,
            "Gui.ActiveDocument.ActiveView.setActiveObject('%s', "
            "App.getDocument('%s').getObject('%s'))",
            PARTKEY,
            this->getObject()->getDocument()->getName(),
            this->getObject()->getNameInDocument()
        );
    }
}

bool ViewProviderPart::doubleClicked()
{
    toggleActivePart();
    return true;
}

QIcon ViewProviderPart::getIcon() const
{
    // the original Part object for this ViewProviderPart
    auto part = this->getObject<App::Part>();
    // the normal case for Std_Part
    const char* pixmap = sPixmap;
    // if it's flagged as an Assembly in its Type, it gets another icon
    if (part->Type.getStrValue() == "Assembly") {
        pixmap = aPixmap;
    }

    return mergeGreyableOverlayIcons(Gui::BitmapFactory().pixmap(pixmap));
}


// Python feature -----------------------------------------------------------------------

namespace Gui
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Gui::ViewProviderPartPython, Gui::ViewProviderPart)
/// @endcond

// explicit template instantiation
template class GuiExport ViewProviderFeaturePythonT<ViewProviderPart>;
}  // namespace Gui
