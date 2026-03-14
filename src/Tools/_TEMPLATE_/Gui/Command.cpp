#include <App/Document.h>
#include <Base/Console.h>
#include <Gui/Application.h>
#include <Gui/Command.h>


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Cmd_TEMPLATE_Test THIS IS JUST A TEST COMMAND
//===========================================================================
DEF_STD_CMD(Cmd_TEMPLATE_Test)

Cmd_TEMPLATE_Test::Cmd_TEMPLATE_Test()
    : Command("_TEMPLATE__Test")
{
    sAppModule = "_TEMPLATE_";
    sGroup = QT_TR_NOOP("_TEMPLATE_");
    sMenuText = QT_TR_NOOP("Hello");
    sToolTipText = QT_TR_NOOP("_TEMPLATE_ Test function");
    sWhatsThis = "_TEMPLATE__Test";
    sStatusTip = QT_TR_NOOP("_TEMPLATE_ Test function");
    sPixmap = "_TEMPLATE_Workbench";
    sAccel = "CTRL+H";
}

void Cmd_TEMPLATE_Test::activated(int)
{
    Base::Console().message("Hello, World!\n");
}

void Create_TEMPLATE_Commands(void)
{
    Gui::CommandManager& rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new Cmd_TEMPLATE_Test());
}
