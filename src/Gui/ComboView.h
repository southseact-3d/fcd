#ifndef GUI_DOCKWND_COMBOVIEW_H
#define GUI_DOCKWND_COMBOVIEW_H

#include "DockWindow.h"


class QTabWidget;
class QTreeView;

namespace App
{
class PropertyContainer;
}

namespace Gui
{
class TreeWidget;
class PropertyView;
class TreePanel;
namespace PropertyEditor
{
class EditableListView;
class EditableItem;
class PropertyEditor;
}  // namespace PropertyEditor

namespace TaskView
{
class TaskView;
class TaskDialog;
}  // namespace TaskView
}  // namespace Gui


namespace Gui
{
class ControlSingleton;
namespace DockWnd
{

/** Combo View
 * is a combination of a tree and property view for
 * integrated user action.
 */
class GuiExport ComboView: public Gui::DockWindow
{
    Q_OBJECT

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    ComboView(Gui::Document* pcDocument, QWidget* parent = nullptr);

    void setShowModel(bool);

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    ~ComboView() override;

    friend class Gui::ControlSingleton;

private:
    Gui::PropertyView* prop;
    Gui::TreePanel* tree;
};

}  // namespace DockWnd
}  // namespace Gui

#endif  // GUI_DOCKWND_SELECTIONVIEW_H
