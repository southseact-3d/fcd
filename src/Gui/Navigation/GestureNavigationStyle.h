#ifndef GESTURENAVIGATIONSTYLE2_H
#define GESTURENAVIGATIONSTYLE2_H

#include "Navigation/NavigationStyle.h"

#include <queue>
#include <memory>

namespace Gui
{


class GestureNavigationStyle: public UserNavigationStyle
{
    using superclass = UserNavigationStyle;

    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    GestureNavigationStyle();
    ~GestureNavigationStyle() override;
    const char* mouseButtons(ViewerMode) override;
    ClarifySelectionMode clarifySelectionMode() const override
    {
        return ClarifySelectionMode::Ctrl;
    }

protected:
    SbBool processSoEvent(const SoEvent* const ev) override;

public:
    /// calls processSoEvent of NavigationStyle.
    SbBool processSoEvent_bypass(const SoEvent* const ev);

protected:  // state machine classes
    /// State machine event, a wrapper around SoEvent
    class Event;

    class NaviMachine;

    class IdleState;
    /// when operating a dragger, for example
    class InteractState;
    /// button was pressed, but the cursor hasn't moved yet
    class AwaitingMoveState;
    /// when in a two-finger touchscreen gestures
    class GestureState;
    /// rotating the viewed model with mouse drag
    class RotateState;
    /// panning with mouse drag
    class PanState;
    /// panning triggered by tap-and-hold. It won't switch to Tilt and Rotate upon
    /// pressing/releasing mouse buttons
    class StickyPanState;
    /// tilting with mouse drag
    class TiltState;
    /// this state discards all mouse input (except detecting button roll gestures), until all
    /// buttons are released.
    class AwaitingReleaseState;

    class EventQueue: public std::queue<SoMouseButtonEvent>
    {
    public:
        EventQueue(GestureNavigationStyle& ns)
            : ns(ns)
        {}

        void post(const Event& ev);
        void discardAll();
        void forwardAll();

    public:
        GestureNavigationStyle& ns;
    };


protected:  // members variables
    std::unique_ptr<NaviMachine> naviMachine;
    EventQueue postponedEvents;

    // settings:
    /// if false, tilting with touchscreen gestures will be disabled
    bool enableGestureTilt = false;
    /// distance in px to treat as a definite drag (noise gate)
    int mouseMoveThreshold = 5;
    /// used by roll gesture detection logic, in AwaitingMoveState and AwaitingReleaseState.
    int rollDir = 0;
    bool logging = false;

public:
    bool is2DViewing() const;

public:  // gesture reactions
    /// Roll gesture is like: press LMB, press RMB, release LMB, release RMB.
    ///  This function is called by state machine whenever it picks up roll gesture.
    void onRollGesture(int direction);
};

}  // namespace Gui
#endif  // GESTURENAVIGATIONSTYLE2_H
