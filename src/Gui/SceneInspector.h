#ifndef GUI_SCENEINSPECTOR_H
#define GUI_SCENEINSPECTOR_H

#include <QDialog>
#include <QHash>
#include <QStandardItemModel>

class SoNode;

namespace Gui
{
class Document;
namespace Dialog
{

class Ui_SceneInspector;

/// Stores data representing scenegraph nodes.
class SceneModel: public QStandardItemModel
{
    Q_OBJECT

public:
    SceneModel(QObject* parent);
    ~SceneModel() override;

    /// Tree structure: column count is 1.
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    /** returns empty QVariant, unless orientation == Qt::Horizontal,
     *  role == Qt::DisplayRole and section == 0 where it returns
     *  "Inventor Tree"
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    /// header data not used: returns false
    bool setHeaderData(
        int section,
        Qt::Orientation orientation,
        const QVariant& value,
        int role = Qt::EditRole
    ) override;
    /// insert the first node in tree
    void setNode(SoNode* node);
    /// set names per node
    void setNodeNames(const QHash<SoNode*, QString>& names);
    /// returns standard parent's flags
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    void setNode(QModelIndex, SoNode*);
    QHash<SoNode*, QString> nodeNames;
};

/// Dialog window to display scenegraph model as a tree
class DlgInspector: public QDialog
{
    Q_OBJECT

public:
    DlgInspector(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgInspector() override;

    void setDocument(Gui::Document* doc);

private:
    void onRefreshButtonClicked();

protected:
    void changeEvent(QEvent* e) override;
    void setNode(SoNode* node);
    void setNodeNames(Gui::Document*);

private:
    Ui_SceneInspector* ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_SCENEINSPECTOR_H
