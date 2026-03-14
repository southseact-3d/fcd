#ifndef SHEETTABLEVIEW_INTERFACE_H
#define SHEETTABLEVIEW_INTERFACE_H

#include <Mod/Spreadsheet/Gui/SheetTableView.h>
#include <QtWidgets/qaccessiblewidget.h>

namespace SpreadsheetGui
{

// Currently SheetTableViewAccessibleInterface below deactivates the
// built-in QAccessibleTable interface, and all the accessibility
// features.
//
// For a proper implementation, start by extending that
// and ensure you're not queue-ing empty cells, or counting empty cells
//
// Otherwise it will hang - https://github.com/FreeCAD/FreeCAD/issues/8265

class SheetTableViewAccessibleInterface: public QAccessibleWidget
{
public:
    explicit SheetTableViewAccessibleInterface(SpreadsheetGui::SheetTableView* view);

    QString text(QAccessible::Text txt) const override;

    QAccessibleInterface* childAt(int x, int y) const override;
    int indexOfChild(const QAccessibleInterface*) const override;
    int childCount() const override;
    QAccessibleInterface* focusChild() const override;
    QAccessibleInterface* child(int index) const override;

    static QAccessibleInterface* ifactory(const QString& key, QObject* obj);
};
}  // namespace SpreadsheetGui

#endif  // SHEETTABLEVIEW_INTERFACE_H
