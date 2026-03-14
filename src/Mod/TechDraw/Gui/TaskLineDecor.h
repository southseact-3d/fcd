#ifndef GUI_TASKVIEW_TASKLINEDECOR_H
#define GUI_TASKVIEW_TASKLINEDECOR_H

#include <Mod/TechDraw/App/DrawViewPart.h>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/TechDraw/TechDrawGlobal.h>
#include <Mod/TechDraw/App/Cosmetic.h>

namespace App
{
class DocumentObject;
}

namespace TechDraw
{
class LineGenerator;
}

namespace TechDrawGui
{
class Ui_TaskLineDecor;
class Ui_TaskRestoreLines;

class TaskLineDecor : public QWidget
{
    Q_OBJECT

public:
    TaskLineDecor(TechDraw::DrawViewPart* partFeat,
                  std::vector<std::string> edgeNames);
    ~TaskLineDecor() override;

public:
    virtual bool accept();
    virtual bool reject();
    bool apply() { return m_apply; }
    void apply(bool b) { m_apply = b; }

protected Q_SLOTS:
    void onStyleChanged();
    void onColorChanged();
    void onWeightChanged();
    void onVisibleChanged();

protected:
    void changeEvent(QEvent *e) override;
    void initUi();

    TechDraw::LineFormat *getFormatAccessPtr(const std::string &edgeName, std::string *newFormatTag = nullptr);
    void initializeRejectArrays();

    void applyDecorations();
    void getDefaults();

private:
    std::unique_ptr<Ui_TaskLineDecor> ui;
    TechDraw::DrawViewPart* m_partFeat;
    std::vector<std::string> m_edges;

    std::vector<TechDraw::LineFormat> m_originalFormats;
    std::vector<std::string> m_createdFormatTags;

    int m_style;
    Base::Color m_color;
    double m_weight;
    bool m_visible;
    bool m_apply;
    int m_lineNumber;

    TechDraw::LineGenerator* m_lineGenerator;
};

class TaskRestoreLines : public QWidget
{
    Q_OBJECT

public:
    TaskRestoreLines(TechDraw::DrawViewPart* partFeat,
                     TechDrawGui::TaskLineDecor* parent);
    ~TaskRestoreLines() override;

public:
    virtual bool accept();
    virtual bool reject();

protected Q_SLOTS:
    void onAllPressed();
    void onGeometryPressed();
    void onCosmeticPressed();
    void onCenterPressed();

protected:
    void changeEvent(QEvent *e) override;
    void initUi();
    int countInvisibleLines();
    int countInvisibleGeoms();
    int countInvisibleCosmetics();
    int countInvisibleCenters();
    void restoreInvisibleLines();
    void restoreInvisibleGeoms();
    void restoreInvisibleCosmetics();
    void restoreInvisibleCenters();

private:
    std::unique_ptr<Ui_TaskRestoreLines> ui;
    TechDraw::DrawViewPart* m_partFeat;
    TaskLineDecor* m_parent;
};


class TaskDlgLineDecor : public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskDlgLineDecor(TechDraw::DrawViewPart* partFeat,
                     std::vector<std::string> edgeNames);
    ~TaskDlgLineDecor() override;

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

protected:

private:
    TaskLineDecor * widget;
    TaskRestoreLines* restore;
    Gui::TaskView::TaskBox* taskbox;
    Gui::TaskView::TaskBox* restoreBox;
};

} //namespace TechDrawGui

#endif // #ifndef GUI_TASKVIEW_TASKLINEDECOR_H
