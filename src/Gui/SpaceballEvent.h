#ifndef SPACEBALLEVENT_H
#define SPACEBALLEVENT_H

#include <QInputEvent>

namespace Spaceball
{
enum ButtonStateType
{
    BUTTON_NONE = 0,
    BUTTON_PRESSED,
    BUTTON_RELEASED
};

class EventBase: public QInputEvent
{
public:
    bool isHandled()
    {
        return handled;
    }
    void setHandled(bool sig)
    {
        handled = sig;
    }

protected:
    explicit EventBase(QEvent::Type event);
    bool handled {false};
};

class MotionEvent: public EventBase
{
public:
    MotionEvent();
    MotionEvent(const MotionEvent& in);
    MotionEvent& operator=(const MotionEvent& in);
    void translations(int& xTransOut, int& yTransOut, int& zTransOut);
    void setTranslations(const int& xTransIn, const int& yTransIn, const int& zTransIn);
    int translationX()
    {
        return xTrans;
    }
    int translationY()
    {
        return yTrans;
    }
    int translationZ()
    {
        return zTrans;
    }

    void rotations(int& xRotOut, int& yRotOut, int& zRotOut);
    void setRotations(const int& xRotIn, const int& yRotIn, const int& zRotIn);
    int rotationX()
    {
        return xRot;
    }
    int rotationY()
    {
        return yRot;
    }
    int rotationZ()
    {
        return zRot;
    }

    static int MotionEventType;

private:
    int xTrans {0};
    int yTrans {0};
    int zTrans {0};
    int xRot {0};
    int yRot {0};
    int zRot {0};
};

class ButtonEvent: public EventBase
{
public:
    ButtonEvent();
    ButtonEvent(const ButtonEvent& in);
    ButtonEvent& operator=(const ButtonEvent& in);
    ButtonStateType buttonStatus();
    void setButtonStatus(const ButtonStateType& buttonStatusIn);
    int buttonNumber();
    void setButtonNumber(const int& buttonNumberIn);

    static int ButtonEventType;

private:
    ButtonStateType buttonState;
    int button;
};
}  // namespace Spaceball
#endif  // SPACEBALLEVENT_H
