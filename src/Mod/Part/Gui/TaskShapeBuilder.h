#ifndef PARTGUI_TASKSHAPEBUILDER_H
#define PARTGUI_TASKSHAPEBUILDER_H

#include <Gui/Selection/Selection.h>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>


namespace PartGui
{

class ShapeBuilderWidget: public QWidget, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    explicit ShapeBuilderWidget(QWidget* parent = nullptr);
    ~ShapeBuilderWidget() override;

    bool accept();
    bool reject();

private:
    void onCreateButtonClicked();
    void onSelectButtonClicked();
    void switchMode(int);

private:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

private:
    void createEdgeFromVertex();
    void createWireFromEdge();
    void createFaceFromVertex();
    void createFaceFromEdge();
    void createShellFromFace();
    void createSolidFromShell();
    void changeEvent(QEvent* e) override;

private:
    class Private;
    Private* d;
};

class TaskShapeBuilder: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskShapeBuilder();
    ~TaskShapeBuilder() override;

public:
    void open() override;
    bool accept() override;
    bool reject() override;
    void clicked(int) override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Close;
    }

private:
    ShapeBuilderWidget* widget;
};

}  // namespace PartGui

#endif  // PARTGUI_TASKSHAPEBUILDER_H
