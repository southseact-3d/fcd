#include <Gui/FileDialog.h>
#include <Gui/MainWindow.h>

#include "DlgPartImportStepImp.h"
#include "ui_DlgPartImportStep.h"


using namespace PartGui;

/* TRANSLATOR PartGui::DlgPartImportStepImp */

/*
 *  Constructs a DlgPartImportStep which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
DlgPartImportStepImp::DlgPartImportStepImp(QWidget* parent, Qt::WindowFlags fl)
    : QDialog(parent, fl)
    , ui(new Ui_DlgPartImportStep)
{
    ui->setupUi(this);
}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgPartImportStepImp::~DlgPartImportStepImp() = default;

/*
 * public slot
 */
void DlgPartImportStepImp::OnApply()
{
    qWarning("DlgPartImportStepImp::OnApply() not yet implemented!");
}

void DlgPartImportStepImp::onChooseFileName()
{
    QString fn = Gui::FileDialog::getOpenFileName(
                     Gui::getMainWindow(),
                     QString(),
                     QString(),
                     QStringLiteral("%1 (*.stp *.step);;%2 (*.*)")
    )
                     .arg(QLatin1String("STEP"), tr("All Files"));
    if (!fn.isEmpty()) {
        ui->FileName->setText(fn);
    }
}


#include "moc_DlgPartImportStepImp.cpp"
