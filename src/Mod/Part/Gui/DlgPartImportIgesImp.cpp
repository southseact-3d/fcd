#include <Gui/FileDialog.h>
#include <Gui/MainWindow.h>

#include "DlgPartImportIgesImp.h"
#include "ui_DlgPartImportIges.h"


using namespace PartGui;

/* TRANSLATOR PartGui::DlgPartImportIgesImp */

/*
 *  Constructs a DlgPartImportIges which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
DlgPartImportIgesImp::DlgPartImportIgesImp(QWidget* parent, Qt::WindowFlags fl)
    : QDialog(parent, fl)
    , ui(new Ui_DlgPartImportIges)
{
    ui->setupUi(this);
}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgPartImportIgesImp::~DlgPartImportIgesImp() = default;

/*
 * public slot
 */
void DlgPartImportIgesImp::OnApply()
{
    qWarning("DlgPartImportIgesImp::OnApply() not yet implemented!");
}

void DlgPartImportIgesImp::onChooseFileName()
{
    QString fn = Gui::FileDialog::getOpenFileName(
                     Gui::getMainWindow(),
                     QString(),
                     QString(),
                     QStringLiteral("%1 (*.igs *.iges);;%2 (*.*)")
    )
                     .arg(tr("IGES"), tr("All Files"));
    if (!fn.isEmpty()) {
        ui->FileName->setText(fn);
    }
}


#include "moc_DlgPartImportIgesImp.cpp"
