#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>


#include "SheetTableViewAccessibleInterface.h"

namespace SpreadsheetGui
{

SheetTableViewAccessibleInterface::SheetTableViewAccessibleInterface(
    SpreadsheetGui::SheetTableView* view
)
    : QAccessibleWidget(view)
{}

QString SheetTableViewAccessibleInterface::text(QAccessible::Text txt) const
{
    if (txt == QAccessible::Help) {
        return QStringLiteral("Implement me");
    }
    return QAccessibleWidget::text(txt);
}

QAccessibleInterface* SheetTableViewAccessibleInterface::childAt(int x, int y) const
{
    Q_UNUSED(x)
    Q_UNUSED(y)
    return (QAccessibleInterface*)this;
}

int SheetTableViewAccessibleInterface::indexOfChild(const QAccessibleInterface*) const
{
    return 0;
}

int SheetTableViewAccessibleInterface::childCount() const
{
    return 0;
}

QAccessibleInterface* SheetTableViewAccessibleInterface::focusChild() const
{
    return (QAccessibleInterface*)this;
}

QAccessibleInterface* SheetTableViewAccessibleInterface::child(int index) const
{
    Q_UNUSED(index)
    return (QAccessibleInterface*)this;
}

QAccessibleInterface* SheetTableViewAccessibleInterface::ifactory(const QString& key, QObject* obj)
{
    if (key == QStringLiteral("SpreadsheetGui::SheetTableView")) {
        return new SheetTableViewAccessibleInterface(static_cast<SpreadsheetGui::SheetTableView*>(obj));
    }
    return nullptr;
}
}  // namespace SpreadsheetGui
