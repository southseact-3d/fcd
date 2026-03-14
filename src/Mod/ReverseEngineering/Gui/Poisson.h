#ifndef REENGUI_POISSON_H
#define REENGUI_POISSON_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>


namespace ReenGui
{

class PoissonWidget: public QWidget
{
    Q_OBJECT

public:
    explicit PoissonWidget(const App::DocumentObjectT&, QWidget* parent = nullptr);
    ~PoissonWidget() override;

    bool accept();

private:
    void changeEvent(QEvent* e) override;

private:
    class Private;
    Private* d;
};

class TaskPoisson: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskPoisson(const App::DocumentObjectT&);
    ~TaskPoisson() override;

public:
    void open() override;
    bool accept() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    PoissonWidget* widget;
};

}  // namespace ReenGui

#endif  // REENGUI_POISSON_H
