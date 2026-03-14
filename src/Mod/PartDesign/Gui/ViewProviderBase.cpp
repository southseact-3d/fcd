#include <App/Document.h>
#include <Gui/CommandT.h>
#include <Mod/PartDesign/App/FeatureBase.h>

#include "ViewProviderBase.h"


using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderBase, PartDesignGui::ViewProvider)

ViewProviderBase::ViewProviderBase()
{
    sPixmap = "PartDesign_BaseFeature.svg";
}

ViewProviderBase::~ViewProviderBase() = default;

bool ViewProviderBase::doubleClicked()
{
    // If the Placement is mutable then open the transform panel.
    // If the Placement can't be modified then just do nothing on double-click.
    PartDesign::FeatureBase* base = getObject<PartDesign::FeatureBase>();
    if (!base->Placement.testStatus(App::Property::Immutable)
        && !base->Placement.testStatus(App::Property::ReadOnly)
        && !base->Placement.testStatus(App::Property::Hidden)) {

        try {
            std::string Msg("Edit ");
            Msg += base->Label.getValue();
            Gui::Command::openCommand(Msg.c_str());
            Gui::cmdSetEdit(base, Gui::Application::Instance->getUserEditMode());
        }
        catch (const Base::Exception&) {
            Gui::Command::abortCommand();
        }
        return true;
    }

    return false;
}

void ViewProviderBase::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    // If the Placement is mutable then show the context-menu of the base class.
    PartDesign::FeatureBase* base = getObject<PartDesign::FeatureBase>();
    if (!base->Placement.testStatus(App::Property::Immutable)
        && !base->Placement.testStatus(App::Property::ReadOnly)
        && !base->Placement.testStatus(App::Property::Hidden)) {

        // Handling of the edge case where some base features are outside the body
        // that should not happen, but it was possible to do in older FreeCAD versions.
        // This ensures that for older files it still works correctly.
        if (!getBodyViewProvider()) {
            ViewProviderPartExt::setupContextMenu(menu, receiver, member);
        }

        ViewProvider::setupContextMenu(menu, receiver, member);
    }
}
Gui::ViewProvider* ViewProviderBase::startEditing(int ModNum)
{
    if (!getBodyViewProvider()) {
        return ViewProviderPartExt::startEditing(ModNum);
    }

    return ViewProvider::startEditing(ModNum);
}

bool ViewProviderBase::setEdit(int ModNum)
{
    PartDesign::FeatureBase* base = getObject<PartDesign::FeatureBase>();
    if (!base->Placement.testStatus(App::Property::Immutable)
        && !base->Placement.testStatus(App::Property::ReadOnly)
        && !base->Placement.testStatus(App::Property::Hidden)) {

        // same as in setupContextMenu
        if (!getBodyViewProvider()) {
            return ViewProviderPartExt::setEdit(ModNum);
        }

        return ViewProvider::setEdit(ModNum);
    }

    return false;
}
