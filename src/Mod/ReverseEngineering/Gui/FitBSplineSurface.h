#ifndef REENGUI_FITBSPLINESURFACE_H
#define REENGUI_FITBSPLINESURFACE_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>


namespace ReenGui
{

class FitBSplineSurfaceWidget: public QWidget
{
    Q_OBJECT

public:
    explicit FitBSplineSurfaceWidget(const App::DocumentObjectT&, QWidget* parent = nullptr);
    ~FitBSplineSurfaceWidget() override;

    bool accept();

private:
    void restoreSettings();
    void saveSettings();
    void changeEvent(QEvent* e) override;

private:
    void onMakePlacementClicked();

private:
    class Private;
    Private* d;
};

class TaskFitBSplineSurface: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskFitBSplineSurface(const App::DocumentObjectT&);

public:
    void open() override;
    bool accept() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    FitBSplineSurfaceWidget* widget;
};

}  // namespace ReenGui

#endif  // REENGUI_FITBSPLINESURFACE_H
