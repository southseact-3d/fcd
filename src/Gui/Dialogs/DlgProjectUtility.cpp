#include <sstream>
#include <QDir>
#include <QMessageBox>

#include <App/Document.h>

#include "Dialogs/DlgProjectUtility.h"
#include "ui_DlgProjectUtility.h"
#include "Application.h"
#include "Command.h"


using namespace Gui::Dialog;


/* TRANSLATOR Gui::Dialog::DlgProjectUtility */

DlgProjectUtility::DlgProjectUtility(QWidget* parent, Qt::WindowFlags fl)
    : QDialog(parent, fl)
    , ui(new Ui_DlgProjectUtility)
{
    ui->setupUi(this);
    connect(ui->extractButton, &QPushButton::clicked, this, &DlgProjectUtility::extractButton);
    connect(ui->createButton, &QPushButton::clicked, this, &DlgProjectUtility::createButton);
    ui->extractSource->setFilter(QStringLiteral("%1 (*.FCStd)").arg(tr("Project file")));
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgProjectUtility::~DlgProjectUtility() = default;

void DlgProjectUtility::extractButton()
{
    QString source = ui->extractSource->fileName();
    QString dest = ui->extractDest->fileName();
    if (source.isEmpty()) {
        QMessageBox::critical(this, tr("Empty source"), tr("No source is defined."));
        return;
    }

    if (dest.isEmpty()) {
        QMessageBox::critical(this, tr("Empty destination"), tr("No destination is defined."));
        return;
    }

    tryExtractArchive(source, dest);
}

void DlgProjectUtility::createButton()
{
    QString source = ui->createSource->fileName();
    QString dest = ui->createDest->fileName();
    if (source.isEmpty()) {
        QMessageBox::critical(this, tr("Empty source"), tr("No source is defined."));
        return;
    }
    if (dest.isEmpty()) {
        QMessageBox::critical(this, tr("Empty destination"), tr("No destination is defined."));
        return;
    }

    dest = QDir(dest).absoluteFilePath(QStringLiteral("project.fcstd"));

    bool openFile = ui->checkLoadProject->isChecked();
    tryCreateArchive(source, dest, openFile);
}

void DlgProjectUtility::tryExtractArchive(const QString& source, const QString& target)
{
    try {
        std::stringstream str;
        str << "from freecad import project_utility\n";
        str << "project_utility.extractDocument(\"" << (const char*)source.toUtf8() << "\", \""
            << (const char*)target.toUtf8() << "\")";
        Gui::Command::runCommand(Gui::Command::App, str.str().c_str());
    }
    catch (const Base::Exception& e) {
        QMessageBox::critical(this, tr("Failed to extract document"), QString::fromLatin1(e.what()));
    }
}

void DlgProjectUtility::tryCreateArchive(const QString& source, const QString& target, bool openFile)
{
    try {
        std::stringstream str;
        str << "from freecad import project_utility\n";
        str << "project_utility.createDocument(\"" << (const char*)source.toUtf8() << "\", \""
            << (const char*)target.toUtf8() << "\")";
        Gui::Command::runCommand(Gui::Command::App, str.str().c_str());
        if (openFile) {
            Application::Instance->open((const char*)target.toUtf8(), "FreeCAD");
        }
    }
    catch (const Base::Exception& e) {
        QMessageBox::critical(this, tr("Failed to create document"), QString::fromLatin1(e.what()));
    }
}

#include "moc_DlgProjectUtility.cpp"
