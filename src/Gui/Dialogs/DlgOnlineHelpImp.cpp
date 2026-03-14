#include <QDir>
#include <QMessageBox>

#include <App/Application.h>

#include "Dialogs/DlgOnlineHelpImp.h"
#include "ui_DlgOnlineHelp.h"


using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgOnlineHelpImp */

/**
 *  Constructs a DlgOnlineHelpImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
DlgOnlineHelpImp::DlgOnlineHelpImp(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgOnlineHelp)
{
    ui->setupUi(this);

    ui->prefStartPage->setFilter(QStringLiteral("%1 (*.html *.htm)").arg(tr("HTML files")));
    if (ui->prefStartPage->fileName().isEmpty()) {
        ui->prefStartPage->setFileName(getStartpage());
    }
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgOnlineHelpImp::~DlgOnlineHelpImp() = default;

/**
 * Returns the start page for the HelpView. If none is defined the default
 * start page "<FreeCADHome>/doc/free-cad.sourceforge.net/wiki/index.php.html"
 * is returned.
 * \remark It is not checked if the returned page really exists.
 */
QString DlgOnlineHelpImp::getStartpage()
{
    QDir docdir = QDir(QString::fromUtf8(App::Application::getHelpDir().c_str()));
    return docdir.absoluteFilePath(QStringLiteral("Start_Page.html"));
}

void DlgOnlineHelpImp::saveSettings()
{
    ui->prefStartPage->onSave();
}

void DlgOnlineHelpImp::loadSettings()
{
    ui->prefStartPage->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgOnlineHelpImp::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

void DlgOnlineHelpImp::onLineEditDownloadFileNameSelected(const QString& url)
{
    QDir dir(url);
    if (dir.exists() && dir.isEmpty()) {
        QMessageBox::critical(
            this,
            tr("Access denied"),
            tr("Access denied to '%1'\n\n"
               "Specify another directory.")
                .arg(url)
        );
    }
}

#include "moc_DlgOnlineHelpImp.cpp"
