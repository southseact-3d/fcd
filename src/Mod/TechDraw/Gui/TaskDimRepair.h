#ifndef TECHDRAW_TASKDIMREPAIR_H
#define TECHDRAW_TASKDIMREPAIR_H

#include <QListWidget>
#include <QTableWidget>

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/TechDraw/App/DrawViewDimension.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


class Ui_TaskDimRepair;

namespace App
{
class DocumentObject;
}

namespace TechDrawGui
{

class TaskDimRepair: public QWidget
{
    Q_OBJECT

public:
    TaskDimRepair(TechDraw::DrawViewDimension* inDvd);
    ~TaskDimRepair() override;

public:
    virtual bool accept();
    virtual bool reject();

protected Q_SLOTS:
    void slotUseSelection();

protected:
    void changeEvent(QEvent* e) override;

    void setUiPrimary();
    void replaceReferences();
    void updateUi();
    void fillList(QListWidget* lwItems, std::vector<std::string> labels,
                  std::vector<std::string> names);
    void loadTableWidget(QTableWidget* tw, TechDraw::ReferenceVector refs);
    void saveDimState();
    void restoreDimState();

private:
    std::unique_ptr<Ui_TaskDimRepair> ui;
    TechDraw::DrawViewDimension* m_dim;

    long int m_saveMeasureType;
    long int m_saveDimType;
    TechDraw::DrawViewPart* m_saveDvp;
    TechDraw::ReferenceVector m_saveRefs2d;
    TechDraw::ReferenceVector m_saveRefs3d;
    TechDraw::ReferenceVector m_toApply2d;
    TechDraw::ReferenceVector m_toApply3d;
};

class TaskDlgDimReference: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskDlgDimReference(TechDraw::DrawViewDimension* inDvd);
    ~TaskDlgDimReference() override;

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
    void helpRequested() override
    {
        return;
    }
    bool isAllowedAlterDocument() const override
    {
        return false;
    }

    void update();

protected:
private:
    TaskDimRepair* widget;
    Gui::TaskView::TaskBox* taskbox;
};

}//namespace TechDrawGui

#endif// #ifndef TECHDRAW_TASKDIMREPAIR_H
