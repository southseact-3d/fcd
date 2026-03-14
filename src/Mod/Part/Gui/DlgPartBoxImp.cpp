#include "DlgPartBoxImp.h"
#include "ui_DlgPartBox.h"


using namespace PartGui;

/*
 *  Constructs a DlgPartBox which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
DlgPartBoxImp::DlgPartBoxImp(QWidget* parent, Qt::WindowFlags fl)
    : Gui::LocationDialogUiImp(new Ui_DlgPartBox, parent, fl)
{}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgPartBoxImp::~DlgPartBoxImp() = default;

#include "moc_DlgPartBoxImp.cpp"
