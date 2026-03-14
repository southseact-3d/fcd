#ifndef REENGUI_FITBSPLINECURVE_H
#define REENGUI_FITBSPLINECURVE_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>


namespace ReenGui
{

class FitBSplineCurveWidget: public QWidget
{
    Q_OBJECT

public:
    explicit FitBSplineCurveWidget(const App::DocumentObjectT&, QWidget* parent = nullptr);
    ~FitBSplineCurveWidget() override;

    bool accept();

protected:
    void changeEvent(QEvent* e) override;

private:
    void toggleParametrizationType(bool on);
    void toggleSmoothing(bool on);
    void tryAccept();
    void exeCommand(const QString&);
    void tryCommand(const QString&);

private:
    class Private;
    Private* d;
};

class TaskFitBSplineCurve: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskFitBSplineCurve(const App::DocumentObjectT&);

public:
    void open() override;
    bool accept() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    FitBSplineCurveWidget* widget;
};

}  // namespace ReenGui

#endif  // REENGUI_FITBSPLINECURVE_H
