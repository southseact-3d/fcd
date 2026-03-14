#ifndef GUI_NAVIGATIONANIMATION_H
#define GUI_NAVIGATIONANIMATION_H

#include "Navigation/NavigationStyle.h"
#include <Inventor/SbRotation.h>
#include <Inventor/SbVec3f.h>
#include <QVariantAnimation>

namespace Gui
{

class GuiExport NavigationAnimation: protected QVariantAnimation
{
    Q_OBJECT
public:
    explicit NavigationAnimation(NavigationStyle* navigation);
    using QVariantAnimation::state;

Q_SIGNALS:
    void interrupted();

protected:
    NavigationStyle* navigation;

    virtual void initialize() = 0;
    virtual void update(const QVariant& value) = 0;
    virtual void onStop(bool finished);

private:
    void updateCurrentValue(const QVariant& value) override;

    friend class NavigationAnimator;
    friend class QObject;
};

class GuiExport FixedTimeAnimation: public NavigationAnimation
{
public:
    explicit FixedTimeAnimation(
        NavigationStyle* navigation,
        const SbRotation& orientation,
        const SbVec3f& rotationCenter,
        const SbVec3f& translation,
        int duration,
        const QEasingCurve::Type easingCurve
    );

private:
    float angularVelocity;   // [rad/ms]
    SbVec3f linearVelocity;  // [/ms]

    SbRotation targetOrientation;
    SbVec3f targetTranslation;

    float prevAngle;
    SbVec3f prevTranslation;

    SbVec3f rotationCenter;
    SbVec3f rotationAxis;

    void initialize() override;
    void update(const QVariant& value) override;
    void onStop(bool finished) override;
};

class GuiExport SpinningAnimation: public NavigationAnimation
{
public:
    explicit SpinningAnimation(NavigationStyle* navigation, const SbVec3f& axis, float velocity);

private:
    SbVec3f rotationAxis;
    float prevAngle;

    void initialize() override;
    void update(const QVariant& value) override;
    void onStop(bool finished) override;
};

}  // namespace Gui

#endif  // GUI_NAVIGATIONANIMATION_H
