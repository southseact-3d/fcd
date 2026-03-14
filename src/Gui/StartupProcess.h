#ifndef GUI_STARTUPPROCESS_H
#define GUI_STARTUPPROCESS_H

#include <FCGlobal.h>
#include <QStringList>

class QApplication;
class QMessageBox;

namespace Gui
{

class Application;
class MainWindow;

class GuiExport StartupProcess
{
public:
    StartupProcess();
    static void setupApplication();
    void execute();

private:
    void setLibraryPath();
    void setStyleSheetPaths();
    void setImagePaths();
    void registerEventType();
    void setThemePaths();
    void setupFileDialog();
};

class GuiExport StartupPostProcess
{
public:
    StartupPostProcess(MainWindow* mw, Application& guiApp, QApplication* app);
    void setLoadFromPythonModule(bool value);
    void execute();

private:
    void setWindowTitle();
    void setProcessMessages();
    void setAutoSaving();
    void setToolBarIconSize();
    void setWheelEventFilter();
    void setLocale();
    void setCursorFlashing();
    void setQtStyle();
    void migrateOldTheme(const std::string& style);
    void checkOpenGL();
    void loadOpenInventor();
    void setBranding();
    void setStyleSheet();
    void autoloadModules(const QStringList& wb);
    void setImportImageFormats();
    void showMainWindow();
    void activateWorkbench();
    void checkParameters();
    void checkVersionMigration() const;

private:
    bool loadFromPythonModule = false;
    MainWindow* mainWindow;
    Application& guiApp;
    QApplication* qtApp;
};


}  // namespace Gui

#endif  // GUI_STARTUPPROCESS_H
