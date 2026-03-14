#include <QCoreApplication>
#include <QCoreApplication>
#include <QLayout>


#include "Manipulator.h"
#include "StartView.h"

#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/MenuManager.h>

#include <gsl/pointers>

DEF_STD_CMD(CmdStart)

CmdStart::CmdStart()
    : Command("Start_Start")
{
    sAppModule = "Start";
    sGroup = QT_TR_NOOP("Start");
    sMenuText = QT_TR_NOOP("&Start Page");
    sToolTipText = QT_TR_NOOP("Displays the start page");
    sWhatsThis = "Start_Start";
    sStatusTip = sToolTipText;
    sPixmap = "StartCommandIcon";
}

void CmdStart::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    auto mw = Gui::getMainWindow();
    auto existingView = mw->findChild<StartGui::StartView*>(QLatin1String("StartView"));
    if (!existingView) {
        existingView = gsl::owner<StartGui::StartView*>(new StartGui::StartView(mw));
        mw->addWindow(existingView);  // Transfers ownership
    }
    Gui::getMainWindow()->setActiveWindow(existingView);
    existingView->show();
}

void StartGui::Manipulator::modifyMenuBar(Gui::MenuItem* menuBar)
{
    Gui::CommandManager& rcCmdMgr = Gui::Application::Instance->commandManager();
    if (!rcCmdMgr.getCommandByName("Start_Start")) {
        auto newCommand = gsl::owner<CmdStart*>(new CmdStart);
        rcCmdMgr.addCommand(newCommand);  // Transfer ownership
    }

    Gui::MenuItem* helpMenu = menuBar->findItem("&Help");
    Gui::MenuItem* loadStart = new Gui::MenuItem();
    Gui::MenuItem* loadSeparator = new Gui::MenuItem();
    loadStart->setCommand("Start_Start");
    loadSeparator->setCommand("Separator");
    Gui::MenuItem* firstItem = helpMenu->findItem("Std_FreeCADUserHub");
    helpMenu->insertItem(firstItem, loadStart);
    helpMenu->insertItem(firstItem, loadSeparator);
}
