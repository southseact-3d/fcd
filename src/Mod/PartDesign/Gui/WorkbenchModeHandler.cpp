// SPDX-License-Identifier: LGPL-2.1-or-later

#include "WorkbenchModeHandler.h"
#include <Gui/ToolBarManager.h>
#include <App/Application.h>
#include <Base/Parameter.h>

using namespace PartDesignGui;

const QString DesignModeKey = QStringLiteral("Design");
const QString MeshModeKey = QStringLiteral("Mesh");
const QString MaterialModeKey = QStringLiteral("Material");

const QString& WorkbenchModeHandler::modeToString(WorkbenchMode mode)
{
    if (mode == WorkbenchMode::DesignMode)
        return DesignModeKey;
    else if (mode == WorkbenchMode::MeshMode)
        return MeshModeKey;
    else
        return MaterialModeKey;
}

WorkbenchMode WorkbenchModeHandler::stringToMode(const QString& str)
{
    if (str == MeshModeKey)
        return WorkbenchMode::MeshMode;
    else if (str == MaterialModeKey)
        return WorkbenchMode::MaterialMode;
    else
        return WorkbenchMode::DesignMode;
}

WorkbenchModeHandler::WorkbenchModeHandler(QObject* parent)
    : QObject(parent)
    , currentMode_(WorkbenchMode::DesignMode)
    , stateSaved(false)
{
    auto hPref = App::GetApplication()
                     .GetUserParameter()
                     .GetGroup("BaseApp")
                     ->GetGroup("Preferences")
                     ->GetGroup("Mod/PartDesign");

    QString savedMode = QString::fromUtf8(
        hPref->GetASCII("LastWorkbenchMode", "Design").c_str());
    currentMode_ = stringToMode(savedMode);
}

WorkbenchModeHandler::~WorkbenchModeHandler() = default;

void WorkbenchModeHandler::saveState() const
{
    Gui::ToolBarManager::getInstance()->saveState();
}

void WorkbenchModeHandler::restoreState()
{
    Gui::ToolBarManager::getInstance()->restoreState();
}

void WorkbenchModeHandler::saveStateForMode(WorkbenchMode mode)
{
    QString modeStr = modeToString(mode);
    auto hPref = App::GetApplication()
                     .GetUserParameter()
                     .GetGroup("BaseApp")
                     ->GetGroup("Preferences")
                     ->GetGroup("Mod/PartDesign");

    hPref->SetASCII("LastWorkbenchMode", modeStr.toUtf8().constData());
}

void WorkbenchModeHandler::restoreStateForMode(WorkbenchMode mode)
{
    auto hPref = App::GetApplication()
                     .GetUserParameter()
                     .GetGroup("BaseApp")
                     ->GetGroup("Preferences")
                     ->GetGroup("Mod/PartDesign");

    QString savedMode = QString::fromUtf8(
        hPref->GetASCII("LastWorkbenchMode", "Design").c_str());
    currentMode_ = stringToMode(savedMode);
}

void WorkbenchModeHandler::switchToMode(WorkbenchMode mode)
{
    if (currentMode_ == mode) {
        return;
    }

    saveStateForMode(currentMode_);

    auto toolBarManager = Gui::ToolBarManager::getInstance();

    QStringList designToolbars = {
        QStringLiteral("Part Design Helper Features"),
        QStringLiteral("Part Design Modeling Features"),
        QStringLiteral("Part Design Dress-Up Features"),
        QStringLiteral("Part Design Transformation Features"),
        QStringLiteral("Part Design Part Tools")
    };

    QStringList meshToolbars = {
        QStringLiteral("Mesh Tools"),
        QStringLiteral("Mesh Modify"),
        QStringLiteral("Mesh Boolean"),
        QStringLiteral("Mesh Cutting"),
        QStringLiteral("Mesh Segmentation"),
        QStringLiteral("Mesh Analyze")
    };

    QStringList materialToolbars = {
        QStringLiteral("Material")
    };

    if (mode == WorkbenchMode::MeshMode) {
        toolBarManager->setState(designToolbars, Gui::ToolBarManager::State::SaveState);
        toolBarManager->setState(designToolbars, Gui::ToolBarManager::State::ForceHidden);
        toolBarManager->setState(materialToolbars, Gui::ToolBarManager::State::SaveState);
        toolBarManager->setState(materialToolbars, Gui::ToolBarManager::State::ForceHidden);
        toolBarManager->setState(meshToolbars, Gui::ToolBarManager::State::ForceAvailable);
    }
    else if (mode == WorkbenchMode::MaterialMode) {
        toolBarManager->setState(designToolbars, Gui::ToolBarManager::State::SaveState);
        toolBarManager->setState(designToolbars, Gui::ToolBarManager::State::ForceHidden);
        toolBarManager->setState(meshToolbars, Gui::ToolBarManager::State::SaveState);
        toolBarManager->setState(meshToolbars, Gui::ToolBarManager::State::ForceHidden);
        toolBarManager->setState(materialToolbars, Gui::ToolBarManager::State::ForceAvailable);
    }
    else {
        toolBarManager->setState(meshToolbars, Gui::ToolBarManager::State::SaveState);
        toolBarManager->setState(meshToolbars, Gui::ToolBarManager::State::ForceHidden);
        toolBarManager->setState(materialToolbars, Gui::ToolBarManager::State::SaveState);
        toolBarManager->setState(materialToolbars, Gui::ToolBarManager::State::ForceHidden);
        toolBarManager->setState(designToolbars, Gui::ToolBarManager::State::ForceAvailable);
    }

    currentMode_ = mode;
    Q_EMIT modeChanged(mode);
}
