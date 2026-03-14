#include <QFileInfo>
#include <QMessageBox>


#include "ModuleIO.h"
#include "Application.h"
#include "FileDialog.h"
#include "MainWindow.h"


using namespace Gui;

/* TRANSLATOR Gui::ModuleIO */

bool ModuleIO::verifyFile(const QString& filename)
{
    QFileInfo fi(filename);
    if (!fi.exists() || !fi.isFile()) {
        QMessageBox::critical(
            Gui::getMainWindow(),
            tr("File not found"),
            tr("The file '%1' cannot be opened.").arg(filename)
        );
        return false;
    }

    return true;
}

void ModuleIO::openFile(const QString& filename)
{
    // invokes appendFile()
    Gui::SelectModule::Dict dict = Gui::SelectModule::importHandler(filename);
    for (Gui::SelectModule::Dict::iterator it = dict.begin(); it != dict.end(); ++it) {
        Gui::Application::Instance->open(it.key().toUtf8(), it.value().toLatin1());
        break;
    }
}

void ModuleIO::verifyAndOpenFile(const QString& filename)
{
    if (verifyFile(filename)) {
        openFile(filename);
    }
}

void ModuleIO::importFiles(const QStringList& filenames, const char* document)
{
    SelectModule::Dict dict = SelectModule::importHandler(filenames);
    // load the files with the associated modules
    for (SelectModule::Dict::iterator it = dict.begin(); it != dict.end(); ++it) {
        // if the passed document name doesn't exist the module should create it, if needed
        Application::Instance->importFrom(it.key().toUtf8(), document, it.value().toLatin1());
    }
}
