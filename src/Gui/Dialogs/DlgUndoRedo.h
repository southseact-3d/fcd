#ifndef GUI_DIALOG_DLGUNDOREDO_H
#define GUI_DIALOG_DLGUNDOREDO_H

#include <QMenu>

namespace Gui
{
namespace Dialog
{

/** This class implements the undo dialog.
 * \author Werner Mayer
 */
class UndoDialog: public QMenu
{
    Q_OBJECT

public:
    explicit UndoDialog(QWidget* parent = nullptr);
    ~UndoDialog() override;

protected Q_SLOTS:
    void onSelected();
    void onFetchInfo();
};

/** This class implements the redo dialog.
 * \author Werner Mayer
 */
class RedoDialog: public QMenu
{
    Q_OBJECT

public:
    explicit RedoDialog(QWidget* parent = nullptr);
    ~RedoDialog() override;

protected Q_SLOTS:
    void onSelected();
    void onFetchInfo();
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGUNDOREDO_H
