#ifndef GUI_COMMAND_COMPLETER_H
#define GUI_COMMAND_COMPLETER_H

#include <FCGlobal.h>
#include <QCompleter>

class QLineEdit;

namespace Gui
{

/**
 * Command name auto completer.
 *
 * This class provides an auto completer for a QLineEdit widget. The auto
 * completer supports keyword search in command title, internal name, and
 * shortcut.
 */
class GuiExport CommandCompleter: public QCompleter
{
    Q_OBJECT
public:
    explicit CommandCompleter(QLineEdit* edit, QObject* parent = nullptr);

Q_SIGNALS:
    /// Triggered when a command is selected in the completer
    void commandActivated(const QByteArray& name);

protected Q_SLOTS:
    void onTextChanged(const QString&);
    void onCommandActivated(const QModelIndex&);

protected:
    bool eventFilter(QObject*, QEvent* ev) override;
};

}  // namespace Gui

#endif
