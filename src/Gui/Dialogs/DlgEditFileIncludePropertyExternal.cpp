#include "Dialogs/DlgEditFileIncludePropertyExternal.h"
#include "Application.h"
#include "FileDialog.h"


using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgEditFileIncludePropertyExternal */

/**
 *  Constructs a DlgEditFileIncludePropertyExternal which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
DlgEditFileIncludePropertyExternal::DlgEditFileIncludePropertyExternal(
    App::PropertyFileIncluded& Prop,
    QWidget* parent,
    Qt::WindowFlags fl
)
    : DlgRunExternal(parent, fl)
    , Prop(Prop)
{}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgEditFileIncludePropertyExternal::~DlgEditFileIncludePropertyExternal() = default;

int DlgEditFileIncludePropertyExternal::processFile()
{
    QFileInfo file(QString::fromUtf8(Prop.getValue()));
    assert(file.exists());

    QDir tmp = QString::fromUtf8(App::Application::getUserCachePath().c_str());
    QString TempFile = tmp.absoluteFilePath(file.fileName());
    QFile::remove(TempFile);

    QFile::copy(file.absoluteFilePath(), TempFile);

    addArgument(TempFile);

    int ret = DlgRunExternal::runProcess();

    if (ret == QDialog::Accepted) {
        Prop.setValue(TempFile.toUtf8());
    }

    QFile::remove(TempFile);
    return ret;
}

#include "moc_DlgEditFileIncludePropertyExternal.cpp"
