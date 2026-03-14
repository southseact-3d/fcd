#ifndef PARTGUI_DLGEXTRUSION_H
#define PARTGUI_DLGEXTRUSION_H

#include <QDialog>
#include <string>

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/Part/App/FeatureExtrusion.h>


class TopoDS_Shape;

namespace PartGui
{

class Ui_DlgExtrusion;
class DlgExtrusion: public QDialog, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    DlgExtrusion(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgExtrusion() override;
    void accept() override;
    void apply();
    void reject() override;

    Base::Vector3d getDir() const;
    void setDir(Base::Vector3d newDir);

    Part::Extrusion::eDirMode getDirMode() const;
    void setDirMode(Part::Extrusion::eDirMode newMode);

    void getAxisLink(App::PropertyLinkSub& lnk) const;
    void setAxisLink(const App::PropertyLinkSub& lnk);
    void setAxisLink(const char* objname, const char* subname);

    std::vector<App::DocumentObject*> getShapesToExtrude() const;

    bool validate();

    void writeParametersToFeature(App::DocumentObject& feature, App::DocumentObject* base) const;

protected:
    void findShapes();
    bool canExtrude(const TopoDS_Shape&) const;
    void changeEvent(QEvent* e) override;
    void keyPressEvent(QKeyEvent*) override;

private:
    void setupConnections();
    void onDirModeCustomToggled(bool on);
    void onDirModeEdgeToggled(bool on);
    void onDirModeNormalToggled(bool on);
    void onSelectEdgeClicked();
    void onButtnoXClicked();
    void onButtonYClicked();
    void onButtonZClicked();
    void onCheckSymmetricToggled(bool on);
    void onTextLinkTextChanged(QString);

private:
    /// updates enabling of controls
    void onDirModeChanged();
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
    /// returns link to any of selected source shapes. Throws if nothing is selected for extrusion.
    App::DocumentObject& getShapeToExtrude() const;
    /// if dirMode is not custom, it tries to compute the actual extrusion direction. Also, it does
    /// some auto-magic manipulation of length value.
    void fetchDir();

    /// automatically checks Solid checkbox depending on input shape
    void autoSolid();

private:
    std::unique_ptr<Ui_DlgExtrusion> ui;
    std::string document, label;
    class EdgeSelection;
    EdgeSelection* filter;
};

class TaskExtrusion: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskExtrusion();

public:
    bool accept() override;
    bool reject() override;
    void clicked(int) override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Close;
    }

private:
    DlgExtrusion* widget;
};

}  // namespace PartGui

#endif  // PARTGUI_DLGEXTRUSION_H
