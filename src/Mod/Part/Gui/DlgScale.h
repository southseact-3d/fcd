#ifndef PARTGUI_DLGSCALE_H
#define PARTGUI_DLGSCALE_H

#include <QDialog>
#include <string>

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/Part/App/FeatureScale.h>

class TopoDS_Shape;

namespace PartGui
{

class Ui_DlgScale;
class DlgScale: public QDialog
{
    Q_OBJECT

public:
    DlgScale(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgScale() = default;
    void accept() override;
    void apply();
    void reject() override;

    std::vector<App::DocumentObject*> getShapesToScale() const;

    bool validate();

    void writeParametersToFeature(App::DocumentObject& feature, App::DocumentObject* base) const;

protected:
    void findShapes();
    bool canScale(const TopoDS_Shape&) const;
    void changeEvent(QEvent* e) override;

private:
    void setupConnections();
    void onUniformScaleToggled(bool on);

private:
    /// returns link to any of selected source shapes. Throws if nothing is selected for scaling.
    App::DocumentObject& getShapeToScale() const;

    std::unique_ptr<Ui_DlgScale> ui;
    std::string m_document, m_label;
};

class TaskScale: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskScale();

public:
    bool accept() override;
    bool reject() override;
    void clicked(int) override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Close;
    }

private:
    DlgScale* widget;
};

}  // namespace PartGui

#endif  // PARTGUI_DLGSCALE_H
