#include <QApplication>
#include <QWhatsThisClickedEvent>


#include "WhatsThis.h"
#include "Action.h"
#include "MainWindow.h"


using namespace Gui;


bool StdCmdDescription::_descrMode = false;

/* TRANSLATOR Gui::StdCmdDescription */

StdCmdDescription::StdCmdDescription()
    : Command("Std_DescriptionMode")
{
    sGroup = "Help";
    sMenuText = QT_TR_NOOP("Des&cription");
    sToolTipText = QT_TR_NOOP("Long description of commands");
    sWhatsThis = "Std_DescriptionMode";
    sStatusTip = sToolTipText;
    sAccel = "F1";
}

StdCmdDescription::~StdCmdDescription() = default;

Action* StdCmdDescription::createAction()
{
    Action* pcAction = Command::createAction();
    pcAction->setCheckable(true);
    return pcAction;
}

void StdCmdDescription::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    if (!inDescriptionMode()) {
        enterDescriptionMode();
    }
    else {
        leaveDescriptionMode();
    }
}

bool StdCmdDescription::inDescriptionMode()
{
    return _descrMode;
}

void StdCmdDescription::setSource(const QString& src)
{
    if (!src.isEmpty()) {
        QWhatsThisClickedEvent e(src);
        QApplication::sendEvent(getMainWindow(), &e);
    }
}

void StdCmdDescription::enterDescriptionMode()
{
    _descrMode = true;
    QApplication::setOverrideCursor(Qt::WhatsThisCursor);
}

void StdCmdDescription::leaveDescriptionMode()
{
    _descrMode = false;
    QApplication::restoreOverrideCursor();
}
