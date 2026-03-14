#ifndef TECHDRAWGUI_TASKACTIVEVIEW_H
#define TECHDRAWGUI_TASKACTIVEVIEW_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


class QPushButton;
class Ui_TaskActiveView;

namespace TechDraw
{
class DrawPage;
class DrawView;
class DrawViewImage;
}

namespace TechDrawGui
{
class QGSPage;
class QGVPage;
class QGIView;
class MDIViewPage;

class TechDrawGuiExport TaskActiveView : public QWidget
{
    Q_OBJECT

public:
    TaskActiveView(TechDraw::DrawPage* pageFeat);
    ~TaskActiveView() override;

public Q_SLOTS:

public:
    virtual bool accept();
    virtual bool reject();
    void updateTask();
    void saveButtons(QPushButton* btnOK,
                     QPushButton* btnCancel);
    void enableTaskButtons(bool b);

protected:
    void changeEvent(QEvent *e) override;

    void blockButtons(bool b);
    void setUiPrimary(void);

    TechDraw::DrawViewImage* createActiveView();
    void enableCrop(bool state);

private Q_SLOTS:
    void onCropChanged();
    void updatePreview();

private:
    std::unique_ptr<Ui_TaskActiveView> ui;

    TechDraw::DrawPage*       m_pageFeat;
    TechDraw::DrawViewImage*  m_imageFeat;
    TechDraw::DrawViewImage* m_previewImageFeat;

    QPushButton* m_btnOK;
    QPushButton* m_btnCancel;
};


class TaskDlgActiveView : public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskDlgActiveView(TechDraw::DrawPage* pageFeat);
    ~TaskDlgActiveView() override;

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

    void modifyStandardButtons(QDialogButtonBox* box) override;

protected:

private:
    TaskActiveView* widget;
    Gui::TaskView::TaskBox* taskbox;

};

} //namespace TechDrawGui

#endif // #ifndef TECHDRAWGUI_TASKACTIVEVIEW_H
