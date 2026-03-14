#ifndef REVERSEENGINEERINGGUI_SEGMENTATIONMANUAL_H
#define REVERSEENGINEERINGGUI_SEGMENTATIONMANUAL_H

#include <memory>

#include <QDialog>

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/Mesh/Gui/MeshSelection.h>


namespace ReverseEngineeringGui
{
class Ui_SegmentationManual;

/**
 * Dialog to create segments from components, regions, the complete or single faces
 * of a mesh.
 * @author Werner Mayer
 */
class SegmentationManual: public QWidget
{
    Q_OBJECT

public:
    explicit SegmentationManual(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~SegmentationManual() override;
    void reject();
    void createSegment();

public:
    void setupConnections();
    void onSelectRegionClicked();
    void onSelectAllClicked();
    void onSelectComponentsClicked();
    void onSelectTriangleClicked();
    void onDeselectAllClicked();
    void onVisibleTrianglesToggled(bool);
    void onScreenTrianglesToggled(bool);
    void onSelectCompToggled(bool);
    void onPlaneDetectClicked();
    void onCylinderDetectClicked();
    void onSphereDetectClicked();

protected:
    void changeEvent(QEvent* e) override;

private:
    class Private;

private:
    std::unique_ptr<Ui_SegmentationManual> ui;
    MeshGui::MeshSelection meshSel;
};

/**
 * Embed the panel into a task dialog.
 */
class TaskSegmentationManual: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskSegmentationManual();

public:
    bool accept() override;
    void clicked(int) override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Close;
    }
    bool isAllowedAlterDocument() const override
    {
        return true;
    }
    void modifyStandardButtons(QDialogButtonBox*) override;

private:
    SegmentationManual* widget;
};

}  // namespace ReverseEngineeringGui

#endif  // REVERSEENGINEERINGGUI_SEGMENTATIONMANUAL_H
