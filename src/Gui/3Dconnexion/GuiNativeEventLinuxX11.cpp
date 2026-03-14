#include <FCConfig.h>
#include "SpaceballEvent.h"

#include <QMainWindow>

#include "GuiNativeEventLinuxX11.h"

#include "GuiApplicationNativeEventAware.h"
#include <Base/Console.h>

#include <QX11Info>
#include <spnav.h>

#include "GuiRawInputEventFilter.h"
#undef Bool
#undef CursorShape
#undef Expose
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef None
#undef Status
#undef Unsorted
#undef False
#undef True
#undef Complex


Gui::GuiNativeEvent::GuiNativeEvent(Gui::GUIApplicationNativeEventAware* app)
    : GuiAbstractNativeEvent(app)
{}

Gui::GuiNativeEvent::~GuiNativeEvent()
{
    if (spnav_close()) {
        Base::Console().log("Couldn't disconnect from spacenav daemon\n");
    }
    else {
        Base::Console().log("Disconnected from spacenav daemon\n");
    }
}

void Gui::GuiNativeEvent::initSpaceball(QMainWindow* window)
{
    if (!QX11Info::isPlatformX11()) {
        Base::Console().log("Application is not running on X11\n");
        return;
    }
    if (spnav_x11_open(QX11Info::display(), window->winId()) == -1) {
        Base::Console()
            .log("Couldn't connect to spacenav daemon on X11. Please ignore if you don't have a spacemouse.\n");
    }
    else {
        spnav_client_name("FreeCAD");
        Base::Console().log("Connected to spacenav daemon on X11\n");
        mainApp->setSpaceballPresent(true);
        mainApp->installNativeEventFilter(new Gui::RawInputEventFilter(&xcbEventFilter));
    }
}

bool Gui::GuiNativeEvent::xcbEventFilter(void* xcb_void, long* result)
{
    Q_UNUSED(result);
    auto inst(dynamic_cast<Gui::GUIApplicationNativeEventAware*>(QApplication::instance()));
    if (!inst) {
        return false;
    }

    spnav_event navEvent;

    const xcb_client_message_event_t* xcb_ev = static_cast<const xcb_client_message_event_t*>(xcb_void);
    // Qt4 used XEvents in native event filters, but Qt5 changed to XCB.  The
    // SpaceNavigator API only works with XEvent, so we need to construct a
    // temporary XEvent with just enough information for spnav_x11_event()
    if ((xcb_ev->response_type & 0x7F) == XCB_CLIENT_MESSAGE) {
        XClientMessageEvent xev;

        xev.type = ClientMessage;
        xev.message_type = xcb_ev->type;
        memcpy(xev.data.b, xcb_ev->data.data8, sizeof(xev.data.b));
        xev.serial = 0;  // These are just to squash warnings...
        xev.send_event = 0;
        xev.display = 0;
        xev.window = 0;
        xev.format = 0;

        if (!spnav_x11_event(reinterpret_cast<XEvent*>(&xev), &navEvent)) {
            return false;
        }
    }
    else {
        return false;
    }
    // navEvent is now initialised

    switch (navEvent.type) {
        case SPNAV_EVENT_MOTION: {
            motionDataArray[0] = -navEvent.motion.x;
            motionDataArray[1] = -navEvent.motion.z;
            motionDataArray[2] = -navEvent.motion.y;
            motionDataArray[3] = -navEvent.motion.rx;
            motionDataArray[4] = -navEvent.motion.rz;
            motionDataArray[5] = -navEvent.motion.ry;

            inst->postMotionEvent(motionDataArray);
            return true;
        }

        case SPNAV_EVENT_BUTTON: {
            auto buttonEvent(new Spaceball::ButtonEvent());
            buttonEvent->setButtonNumber(navEvent.button.bnum);
            if (navEvent.button.press) {
                buttonEvent->setButtonStatus(Spaceball::BUTTON_PRESSED);
            }
            else {
                buttonEvent->setButtonStatus(Spaceball::BUTTON_RELEASED);
            }
            inst->postButtonEvent(navEvent.button.bnum, navEvent.button.press);
            return true;
        }
        default:
            Base::Console().log("Unknown spaceball event\n");
            return true;
    }  // end switch (navEvent.type) {
}

#include "3Dconnexion/moc_GuiNativeEventLinuxX11.cpp"
