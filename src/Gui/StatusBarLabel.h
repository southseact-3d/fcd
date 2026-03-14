#ifndef STATUSBARLABEL_H
#define STATUSBARLABEL_H

#include <string>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QLabel>
#include <QStatusBar>
#include <QMenu>

#include <FCGlobal.h>

#include <Base/Parameter.h>

namespace Gui
{
/**
 * @brief Label for displaying information in the status bar
 *
 * A QLabel subclass that provides a context menu with additional actions
 * similar to the standard status bar widgets.
 */
class GuiExport StatusBarLabel: public QLabel
{
    Q_OBJECT
public:
    explicit StatusBarLabel(QWidget* parent, const std::string& parameterName = {});

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void setVisible(bool visible) override;

private:
    ParameterGrp::handle hGrp;
    std::string parameterName;
};

}  // Namespace Gui
#endif  // STATUSBARLABEL_H
