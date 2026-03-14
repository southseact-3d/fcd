#ifndef GUI_TASKVIEW_TaskHelixParameters_H
#define GUI_TASKVIEW_TaskHelixParameters_H

#include <Gui/Inventor/Draggers/Gizmo.h>

#include "TaskSketchBasedParameters.h"
#include "ViewProviderHelix.h"


namespace App
{
class Property;
}

namespace Gui
{
class LinearGizmo;
class GizmoContainer;
class ViewProvider;
}  // namespace Gui

namespace PartDesignGui
{
class Ui_TaskHelixParameters;


class TaskHelixParameters: public TaskSketchBasedParameters
{
    Q_OBJECT

public:
    explicit TaskHelixParameters(ViewProviderHelix* HelixView, QWidget* parent = nullptr);
    ~TaskHelixParameters() override;

    void apply() override;

    static bool showPreview(PartDesign::Helix*);

private:
    /**
     * @brief fillAxisCombo fills the combo and selects the item according to
     * current value of revolution object's axis reference.
     * @param forceRefill if true, the combo box will be completely refilled. If
     * false, the current value of revolution object's axis will be added to the
     * list (if necessary), and selected. If the list is empty, it will be refilled anyway.
     */
    void fillAxisCombo(bool forceRefill = false);
    void addAxisToCombo(App::DocumentObject* linkObj, std::string linkSubname, QString itemText);
    void addSketchAxes();
    void addPartAxes();
    int addCurrentLink();
    void assignToolTipsFromPropertyDocs();
    void adaptVisibilityToMode();

private Q_SLOTS:
    void onPitchChanged(double);
    void onHeightChanged(double);
    void onTurnsChanged(double);
    void onAngleChanged(double);
    void onGrowthChanged(double);
    void onAxisChanged(int);
    void onLeftHandedChanged(bool);
    void onReversedChanged(bool);
    void onModeChanged(int);
    void onOutsideChanged(bool);


protected:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
    void changeEvent(QEvent* e) override;
    bool updateView() const;
    void getReferenceAxis(App::DocumentObject*& obj, std::vector<std::string>& sub) const;
    void startReferenceSelection(App::DocumentObject* profile, App::DocumentObject* base) override;
    void finishReferenceSelection(App::DocumentObject* profile, App::DocumentObject* base) override;

    // mirrors of helixes's properties
    App::PropertyLength* propPitch;
    App::PropertyLength* propHeight;
    App::PropertyFloatConstraint* propTurns;
    App::PropertyBool* propLeftHanded;
    App::PropertyBool* propReversed;
    App::PropertyLinkSub* propReferenceAxis;
    App::PropertyAngle* propAngle;
    App::PropertyDistance* propGrowth;
    App::PropertyEnumeration* propMode;
    App::PropertyBool* propOutside;


private:
    void initializeHelix();
    void connectSlots();
    void updateUI();
    void updateStatus();
    void assignProperties();
    void setValuesFromProperties();
    void bindProperties();
    void showCoordinateAxes();

private:
    QWidget* proxy;
    std::unique_ptr<Ui_TaskHelixParameters> ui;

    /**
     * @brief axesInList is the list of links corresponding to axis combo; must
     * be kept in sync with the combo. A special value of zero-pointer link is
     * for "Select axis" item.
     *
     * It is a list of pointers, because properties prohibit assignment. Use new
     * when adding stuff, and delete when removing stuff.
     */
    std::vector<std::unique_ptr<App::PropertyLinkSub>> axesInList;

    std::unique_ptr<Gui::GizmoContainer> gizmoContainer;
    Gui::LinearGizmo* heightGizmo = nullptr;
    void setupGizmos(ViewProviderHelix* vp);
    void setGizmoPositions();
};

/// simulation dialog for the TaskView
class TaskDlgHelixParameters: public TaskDlgSketchBasedParameters
{
    Q_OBJECT

public:
    explicit TaskDlgHelixParameters(ViewProviderHelix* HelixView);
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TaskHelixParameters_H
