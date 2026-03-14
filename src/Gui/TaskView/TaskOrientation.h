#ifndef GUI_TASKORIENTATION_H
#define GUI_TASKORIENTATION_H

#include <Gui/TaskView/TaskDialog.h>
#include <App/DocumentObserver.h>
#include <App/GeoFeature.h>
#include <memory>

namespace Gui
{

class Ui_TaskOrientation;
class TaskOrientation: public QWidget
{
    Q_OBJECT

public:
    explicit TaskOrientation(App::GeoFeature* obj, QWidget* parent = nullptr);
    ~TaskOrientation() override;

    void open();
    void accept();
    void reject();

private:
    void restore(const Base::Placement&);
    void onPreview();
    void updateIcon();
    void updatePlacement();

private:
    std::unique_ptr<Ui_TaskOrientation> ui;
    App::WeakPtrT<App::GeoFeature> feature;
};

class TaskOrientationDialog: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskOrientationDialog(App::GeoFeature* obj);

public:
    void open() override;
    bool accept() override;
    bool reject() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    TaskOrientation* widget;
};

}  // namespace Gui

#endif  // GUI_TASKORIENTATION_H
