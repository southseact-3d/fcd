#ifndef GUI_EDITABLEDATUMLABEL_H
#define GUI_EDITABLEDATUMLABEL_H

#include <QObject>
#include <QPointer>
#include <QLabel>
#include <Gui/QuantitySpinBox.h>

#include "SoDatumLabel.h"

#include <FCGlobal.h>

class SoNodeSensor;
class SoTransform;
class SoAnnotation;
class SoSwitch;

namespace Gui
{

class View3DInventorViewer;


class GuiExport EditableDatumLabel: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(EditableDatumLabel)

public:
    enum class Function
    {
        Positioning,
        Dimensioning,
        Forced
    };

    EditableDatumLabel(
        View3DInventorViewer* view,
        const Base::Placement& plc,
        SbColor color,
        bool autoDistance = false,
        bool avoidMouseCursor = false
    );

    ~EditableDatumLabel() override;

    void activate();
    void deactivate();

    void startEdit(double val, QObject* eventFilteringObj = nullptr, bool visibleToMouse = false);
    void stopEdit();
    bool isActive() const;
    bool isInEdit() const;
    double getValue() const;
    void setSpinboxValue(double val, const Base::Unit& unit = Base::Unit::Length);
    void setPlacement(const Base::Placement& plc);
    void setColor(SbColor color);
    void setFocus();
    void setPoints(SbVec3f p1, SbVec3f p2);
    void setPoints(Base::Vector3d p1, Base::Vector3d p2);
    void setFocusToSpinbox();
    void clearSelection();  ///< Clears text selection in the spinbox
    void setLabelType(SoDatumLabel::Type type, Function function = Function::Positioning);
    void setLabelDistance(double val);
    void setLabelStartAngle(double val);
    void setLabelRange(double val);
    void setLabelRecommendedDistance();
    void setLabelAutoDistanceReverse(bool val);
    void setSpinboxVisibleToMouse(bool val);
    void setLockedAppearance(bool locked);  ///< Sets visual appearance to indicate locked state
                                            ///< (finished editing)
    void resetLockedState();  ///< Resets both hasFinishedEditing flag and locked appearance

    Function getFunction();

    // NOLINTBEGIN
    SoDatumLabel* label;
    bool isSet;  ///< used to differentiate when user has started typing inside the label
    bool hasFinishedEditing;  ///< flag to know when user has finished editing, i.e. pressed enter
    bool autoDistance;
    bool autoDistanceReverse;
    bool avoidMouseCursor;
    double value;
    // NOLINTEND

Q_SIGNALS:
    void valueChanged(double val);
    void parameterUnset();
    void finishEditingOnAllOVPs();  ///< Emitted when Ctrl+Enter is pressed to finish editing on all
                                    ///< visible OVPs

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void positionSpinbox();
    SbVec3f getTextCenterPoint() const;

private:
    SoSwitch* root;
    SoAnnotation* annotation;
    SoTransform* transform;
    QPointer<View3DInventorViewer> viewer;
    QuantitySpinBox* spinBox;
    QLabel* lockIconLabel;  ///< Label to display lock icon next to spinbox
    SoNodeSensor* cameraSensor;
    SbVec3f midpos;

    Function function;
};

}  // namespace Gui


#endif  // GUI_EDITABLEDATUMLABEL_H
