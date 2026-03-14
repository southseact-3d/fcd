#ifndef SURFACEGUI_TASKBLENDCURVE_H
#define SURFACEGUI_TASKBLENDCURVE_H

#include <memory>

#include <Gui/DocumentObserver.h>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>


class QListWidgetItem;

namespace Gui
{
class ButtonGroup;
}

namespace SurfaceGui
{

class ViewProviderBlendCurve;
class Ui_BlendCurve;

class BlendCurvePanel: public QWidget, public Gui::SelectionObserver
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BlendCurvePanel)

private:
    std::unique_ptr<Ui_BlendCurve> ui;
    Gui::WeakPtrT<ViewProviderBlendCurve> vp;

public:
    explicit BlendCurvePanel(ViewProviderBlendCurve* vp);
    ~BlendCurvePanel() override;

    void open();
    void checkOpenCommand();
    bool accept();
    bool reject();

protected:
    void changeEvent(QEvent* e) override;
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

private:
    void setupConnections();
    void initControls();
    void initSubLinks();
    void initContinuity();
    void initParameter();
    void initSize();
    void bindProperties();
    void onFirstEdgeButton(bool checked);
    void onSecondEdgeButton(bool checked);
    void onUncheckFirstEdgeButton();
    void onUncheckSecondEdgeButton();

    void onFirstEdgeContChanged(int index);
    void onSecondEdgeContChanged(int index);

    void onFirstEdgeParameterChanged(double value);
    void onSecondEdgeParameterChanged(double value);

    void onFirstEdgeSizeChanged(double value);
    void onSecondEdgeSizeChanged(double value);

    void onStartSelection();
    void clearSelection();
    void exitSelectionMode();
    void setStartEdge(App::DocumentObject* obj, const std::string& subname);
    void setEndEdge(App::DocumentObject* obj, const std::string& subname);
    static QString linkToString(const App::PropertyLinkSub& link);

    class EdgeSelection;
    enum SelectionMode
    {
        None,
        StartEdge,
        EndEdge
    };
    SelectionMode selectionMode = None;
};

class TaskBlendCurve: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskBlendCurve(ViewProviderBlendCurve* vp);

public:
    void open() override;
    bool accept() override;
    bool reject() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    BlendCurvePanel* widget;
};

}  // namespace SurfaceGui

#endif  // SURFACEGUI_TASKBLENDCURVE_H
