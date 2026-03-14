#ifndef TECHDRAWGUI_TASKSELECTLINEATTRIBUTES_H
#define TECHDRAWGUI_TASKSELECTLINEATTRIBUTES_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/TechDraw/TechDrawGlobal.h>
#include <Mod/TechDraw/App/Cosmetic.h>


class dimAttributes {
    double cascadeSpacing;
    double lineStretch;

    public:

    dimAttributes();
    void setCascadeSpacing(double);
    double getCascadeSpacing() {return cascadeSpacing;}
    void setLineStretch(double);
    double getLineStretch() {return lineStretch;}

}; // class dimAttributes

extern dimAttributes activeDimAttributes; // container holding dimension attributes

namespace TechDraw
{
class DrawPage;
class DrawView;
class DrawViewPart;
class CosmeticEdge;
class LineFormat;
}

namespace TechDraw
{
class Face;
class LineGenerator;
}

namespace TechDrawGui
{
class QGSPage;
class QGVPage;
class QGIView;
class QGIPrimPath;
class MDIViewPage;
class ViewProviderViewPart;
class Ui_TaskSelectLineAttributes;

// class lineAttributes {
//     int style;
//     int width;
//     int color;

// public:

//     lineAttributes();
//     void setStyle(int);
//     int getStyle() const {return style;}
//     void setWidth(int);
//     int getWidth() const {return width;}
//     float getWidthValue();
//     void setColor(int);
//     int getColor() const {return color;}
//     Base::Color getColorValue();

// }; // class lineAttributes

class TaskSelectLineAttributes : public QWidget
{
    Q_OBJECT

public:
    explicit TaskSelectLineAttributes();
    ~TaskSelectLineAttributes() override;

    virtual bool accept();
    virtual bool reject();
    void updateTask();

protected:
    void changeEvent(QEvent *event) override;

    void setUiEdit();

private:
    std::unique_ptr<Ui_TaskSelectLineAttributes> ui;

    TechDraw::LineGenerator* m_lineGenerator;
}; // class TaskSelectLineAttributes

class TaskDlgSelectLineAttributes : public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskDlgSelectLineAttributes();
    ~TaskDlgSelectLineAttributes() override;

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
    bool isAllowedAlterDocument() const override
                        { return false; }
    void update();

protected:

private:
    TaskSelectLineAttributes* widget;

    Gui::TaskView::TaskBox* taskbox;
}; // class TaskDlgSelectLineAttributes

} // namespace TechDrawGui

#endif // #ifndef TECHDRAWGUI_TASKSELECTLINEATTRIBUTES_H
