#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
// #include <Mod/Assembly/App/AssemblyObject.h>

#include "TaskAssemblyMessages.h"
#include "ViewProviderAssembly.h"

using namespace AssemblyGui;
using namespace Gui::TaskView;
namespace sp = std::placeholders;

TaskAssemblyMessages::TaskAssemblyMessages(ViewProviderAssembly* vp)
    : TaskSolverMessages(Gui::BitmapFactory().pixmap("Geoassembly"), tr("Solver messages"))
    , vp(vp)
{
    // NOLINTBEGIN
    connectionSetUp = vp->signalSetUp.connect(
        std::bind(&TaskAssemblyMessages::slotSetUp, this, sp::_1, sp::_2, sp::_3, sp::_4)
    );
    // NOLINTEND
}

TaskAssemblyMessages::~TaskAssemblyMessages()
{
    connectionSetUp.disconnect();
}

void TaskAssemblyMessages::updateToolTip(const QString& link)
{
    if (link == QStringLiteral("#conflicting")) {
        setLinkTooltip(tr("Click to select these conflicting joints."));
    }
    else if (link == QStringLiteral("#redundant")) {
        setLinkTooltip(tr("Click to select these redundant joints."));
    }
    else if (link == QStringLiteral("#dofs")) {
        setLinkTooltip(tr(
            "The assembly has unconstrained components giving rise to those "
            "Degrees Of Freedom.\nClick to select these unconstrained components.\nNote: Currently "
            "this selects only unconnected parts, not constrained parts that still have free "
            "DoF."
        ));
    }
    else if (link == QStringLiteral("#malformed")) {
        setLinkTooltip(tr("Click to select these malformed joints."));
    }
}

void TaskAssemblyMessages::onLabelStatusLinkClicked(const QString& str)
{
    if (str == QStringLiteral("#conflicting")) {
        Gui::Application::Instance->commandManager().runCommandByName(
            "Assembly_SelectConflictingConstraints"
        );
    }
    else if (str == QStringLiteral("#redundant")) {
        Gui::Application::Instance->commandManager().runCommandByName(
            "Assembly_SelectRedundantConstraints"
        );
    }
    else if (str == QStringLiteral("#dofs")) {
        Gui::Application::Instance->commandManager().runCommandByName(
            "Assembly_SelectComponentsWithDoFs"
        );
    }
    else if (str == QStringLiteral("#malformed")) {
        Gui::Application::Instance->commandManager().runCommandByName(
            "Assembly_SelectMalformedConstraints"
        );
    }
}

#include "moc_TaskAssemblyMessages.cpp"
