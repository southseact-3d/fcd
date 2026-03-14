#ifndef MESHGUI_SELECTION_H
#define MESHGUI_SELECTION_H

#include <Mod/Mesh/MeshGlobal.h>

#include <vector>
#include <QWidget>

#include <Gui/Selection/SelectionObject.h>

#include "MeshSelection.h"


namespace MeshGui
{

class Ui_Selection;
class Selection: public QWidget
{
    Q_OBJECT

public:
    explicit Selection(QWidget* parent = nullptr);
    ~Selection() override;
    void setObjects(const std::vector<Gui::SelectionObject>&);
    std::vector<App::DocumentObject*> getObjects() const;
    bool eventFilter(QObject*, QEvent*) override;

private:
    void setupConnections();
    void onAddSelectionClicked();
    void onClearSelectionClicked();
    void onVisibleTrianglesToggled(bool);
    void onScreenTrianglesToggled(bool);

private:
    MeshSelection meshSel;
    Ui_Selection* ui;

    Q_DISABLE_COPY_MOVE(Selection)
};

}  // namespace MeshGui

#endif  // MESHGUI_SELECTION_H
