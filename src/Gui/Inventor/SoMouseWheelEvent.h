#ifndef SOMOUSEWHEELEVENT_H_FC
#define SOMOUSEWHEELEVENT_H_FC


#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoSubEvent.h>
#include <FCGlobal.h>

/**
 * @brief The SoMouseWheelEvent class is a temporary replacement for
 * SoMouseWheelEvent from Coin, for until freecad stops using Coin version that
 * doesn't have one (coin v 4.0.0a doesn't have SoMouseWheelEvent).
 */
class GuiExport SoMouseWheelEvent: public SoEvent
{
    SO_EVENT_HEADER();

public:  // methods
    static void initClass()
    {
        SO_EVENT_INIT_CLASS(SoMouseWheelEvent, SoEvent);
    }
    SoMouseWheelEvent()
        : delta(0)
    {}
    SoMouseWheelEvent(int delta)
        : delta(delta)
    {}
    /// returns wheel position change. One click is usually 120 units,
    ///  smaller values come from high-resolution devices like touchpads
    int getDelta() const
    {
        return delta;
    }
    void setDelta(int delta)
    {
        this->delta = delta;
    }
    ~SoMouseWheelEvent() override = default;

private:  // data
    int delta;
};

#endif
