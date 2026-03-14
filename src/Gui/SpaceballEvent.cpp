#include "SpaceballEvent.h"


using namespace Spaceball;

int MotionEvent::MotionEventType = -1;
int ButtonEvent::ButtonEventType = -1;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
EventBase::EventBase(QEvent::Type event)
    : QInputEvent(static_cast<QEvent::Type>(event))
#else
EventBase::EventBase(QEvent::Type event)
    : QInputEvent(static_cast<QEvent::Type>(event), QPointingDevice::primaryPointingDevice())
#endif
{}

MotionEvent::MotionEvent()
    : EventBase(static_cast<QEvent::Type>(MotionEventType))
{}

MotionEvent::MotionEvent(const MotionEvent& in)
    : EventBase(static_cast<QEvent::Type>(MotionEventType))
{
    xTrans = in.xTrans;
    yTrans = in.yTrans;
    zTrans = in.zTrans;
    xRot = in.xRot;
    yRot = in.yRot;
    zRot = in.zRot;
    handled = in.handled;
}

MotionEvent& MotionEvent::operator=(const MotionEvent& in)
{
    if (this == &in) {
        return *this;
    }

    xTrans = in.xTrans;
    yTrans = in.yTrans;
    zTrans = in.zTrans;
    xRot = in.xRot;
    yRot = in.yRot;
    zRot = in.zRot;
    handled = in.handled;
    return *this;
}

void MotionEvent::translations(int& xTransOut, int& yTransOut, int& zTransOut)
{
    xTransOut = xTrans;
    yTransOut = yTrans;
    zTransOut = zTrans;
}

void MotionEvent::setTranslations(const int& xTransIn, const int& yTransIn, const int& zTransIn)
{
    xTrans = xTransIn;
    yTrans = yTransIn;
    zTrans = zTransIn;
}

void MotionEvent::rotations(int& xRotOut, int& yRotOut, int& zRotOut)
{
    xRotOut = xRot;
    yRotOut = yRot;
    zRotOut = zRot;
}

void MotionEvent::setRotations(const int& xRotIn, const int& yRotIn, const int& zRotIn)
{
    xRot = xRotIn;
    yRot = yRotIn;
    zRot = zRotIn;
}


ButtonEvent::ButtonEvent()
    : EventBase(static_cast<QEvent::Type>(ButtonEventType))
    , buttonState(BUTTON_NONE)
    , button(0)
{}

ButtonEvent::ButtonEvent(const ButtonEvent& in)
    : EventBase(static_cast<QEvent::Type>(ButtonEventType))
{
    buttonState = in.buttonState;
    button = in.button;
    handled = in.handled;
}

ButtonEvent& ButtonEvent::operator=(const ButtonEvent& in)
{
    if (this == &in) {
        return *this;
    }

    buttonState = in.buttonState;
    button = in.button;
    handled = in.handled;
    return *this;
}

ButtonStateType ButtonEvent::buttonStatus()
{
    return buttonState;
}

void ButtonEvent::setButtonStatus(const ButtonStateType& buttonStatusIn)
{
    buttonState = buttonStatusIn;
}

int ButtonEvent::buttonNumber()
{
    return button;
}

void ButtonEvent::setButtonNumber(const int& buttonNumberIn)
{
    button = buttonNumberIn;
}
