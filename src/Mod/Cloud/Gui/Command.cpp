#include <Base/Console.h>
#include <Gui/Application.h>
#include <Gui/Command.h>


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// CmdCloudTest THIS IS JUST A TEST COMMAND
//===========================================================================
DEF_STD_CMD(CmdCloudTest)

CmdCloudTest::CmdCloudTest()
    : Command("Cloud_Test")
{
    sAppModule = "Cloud";
    sGroup = QT_TR_NOOP("Cloud");
    sMenuText = QT_TR_NOOP("Hello");
    sToolTipText = QT_TR_NOOP("Cloud Test function");
    sWhatsThis = "Cloud_Test";
    sStatusTip = QT_TR_NOOP("Cloud Test function");
    sPixmap = "Test1";
    sAccel = "CTRL+H";
}

void CmdCloudTest::activated(int)
{
    Base::Console().message("Hello, World!\n");
}

void CreateCloudCommands(void)
{
    Gui::CommandManager& rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdCloudTest());
}
