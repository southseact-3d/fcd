#ifndef GUI_TASKVIEW_TASKDIMENSION_H
#define GUI_TASKVIEW_TASKDIMENSION_H

#include <Gui/DocumentObserver.h>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDrawGui
{

class QGIViewDimension;
class ViewProviderDimension;
class Ui_TaskDimension;
class TaskDimension : public QWidget
{
    Q_OBJECT

public:
    TaskDimension(QGIViewDimension *parent, ViewProviderDimension *dimensionVP);
    ~TaskDimension() override;

public:
    virtual bool accept();
    virtual bool reject();
    void recomputeFeature();

private Q_SLOTS:
    void onTheoreticallyExactChanged();
    void onEqualToleranceChanged();
    void onOvertoleranceChanged();
    void onUndertoleranceChanged();
    void onFormatSpecifierChanged();
    void onArbitraryChanged();
    void onFormatSpecifierOverToleranceChanged();
    void onFormatSpecifierUnderToleranceChanged();
    void onArbitraryTolerancesChanged();
    void onFlipArrowheadsChanged();
    void onColorChanged();
    void onFontsizeChanged();
    void onDrawingStyleChanged();
    void onOverrideToggled();
    void onDimAngleChanged();
    void onExtAngleChanged();
    void onDimUseDefaultClicked();
    void onDimUseSelectionClicked();
    void onExtUseDefaultClicked();
    void onExtUseSelectionClicked();
    void onNumDecChanged(int decimals);
    void onReferenceChanged();

private:
    std::unique_ptr<Ui_TaskDimension> ui;
    QGIViewDimension *m_parent;
    Gui::WeakPtrT<ViewProviderDimension> m_dimensionVP;
    std::pair<double, bool> getAngleFromSelection();
    std::string m_originalFormatChar;
    std::string m_formatPrefix;
    std::string m_formatSuffix;
};

class TaskDlgDimension : public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskDlgDimension(QGIViewDimension *parent, ViewProviderDimension *dimensionVP);
    ~TaskDlgDimension() override;

public:
    /// is called the TaskView when the dialog is opened
    void open() override;
    /// is called by the framework if an button is clicked which has no accept or reject role
    void clicked(int) override;
    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
    /// is called by the framework if the dialog is rejected (Cancel)
    bool reject() override;
    /// is called by the framework if the user presses the help button
    void helpRequested() override { return;}
    bool isAllowedAlterDocument() const override
    { return false; }

    void update();

protected:

private:
    TaskDimension* widget;
    Gui::TaskView::TaskBox* taskbox;
};

} //namespace TechDrawGui

#endif // #ifndef GUI_TASKVIEW_TASKDIMENSION_H
