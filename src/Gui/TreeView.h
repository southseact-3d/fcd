#ifndef GUI_TREEVIEW_H
#define GUI_TREEVIEW_H

#include <QTreeView>
#include <FCGlobal.h>

namespace Gui
{

class GuiExport TreeView: public QTreeView
{
    Q_OBJECT

public:
    TreeView(QWidget* parent = nullptr);
    ~TreeView() override;

protected:
    void mouseDoubleClickEvent(QMouseEvent*) override;
    void rowsInserted(const QModelIndex& parent, int start, int end) override;
};

}  // namespace Gui


#endif  // GUI_TREEVIEW_H
