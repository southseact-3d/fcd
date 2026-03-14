#ifndef PARTGUI_CROSSSECTIONS_H
#define PARTGUI_CROSSSECTIONS_H

#include <QDialog>
#include <QPointer>

#include <Base/BoundBox.h>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>


namespace Gui
{
class View3DInventor;
}

namespace PartGui
{

class ViewProviderCrossSections;
class Ui_CrossSections;
class CrossSections: public QDialog
{
    Q_OBJECT

    enum Plane
    {
        XY,
        XZ,
        YZ
    };

public:
    explicit CrossSections(
        const Base::BoundBox3d& bb,
        QWidget* parent = nullptr,
        Qt::WindowFlags fl = Qt::WindowFlags()
    );
    ~CrossSections() override;
    void accept() override;
    bool apply();

protected:
    void changeEvent(QEvent* e) override;
    void keyPressEvent(QKeyEvent*) override;

private:
    void setupConnections();
    void xyPlaneClicked();
    void xzPlaneClicked();
    void yzPlaneClicked();
    void positionValueChanged(double);
    void distanceValueChanged(double);
    void countSectionsValueChanged(int);
    void checkBothSidesToggled(bool);
    void sectionsBoxToggled(bool);

private:
    std::vector<double> getPlanes() const;
    void calcPlane(Plane, double);
    void calcPlanes(Plane /*, double, bool, int*/);
    void makePlanes(Plane, const std::vector<double>&, double[4]);
    Plane plane() const;

private:
    std::unique_ptr<Ui_CrossSections> ui;
    Base::BoundBox3d bbox;
    ViewProviderCrossSections* vp;
    QPointer<Gui::View3DInventor> view;
};

class TaskCrossSections: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskCrossSections(const Base::BoundBox3d& bb);

public:
    bool accept() override;
    void clicked(int id) override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel;
    }

private:
    CrossSections* widget;
};

}  // namespace PartGui

#endif  // PARTGUI_CROSSSECTIONS_H
