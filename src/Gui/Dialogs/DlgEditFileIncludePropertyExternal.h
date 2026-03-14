#ifndef GUI_DIALOG_DlgEditFileIncludePropertyExternal_H
#define GUI_DIALOG_DlgEditFileIncludePropertyExternal_H

#include "Dialogs/DlgRunExternal.h"
#include <App/PropertyFile.h>

namespace Gui
{
namespace Dialog
{

/**
 *
 * \author Jürgen Riegel
 */
class GuiExport DlgEditFileIncludePropertyExternal: public DlgRunExternal
{
    Q_OBJECT

public:
    DlgEditFileIncludePropertyExternal(
        App::PropertyFileIncluded& Prop,
        QWidget* parent = nullptr,
        Qt::WindowFlags fl = Qt::WindowFlags()
    );
    ~DlgEditFileIncludePropertyExternal() override;

    int processFile();

private:
    App::PropertyFileIncluded& Prop;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DlgEditFileIncludePropertyExternal_H
