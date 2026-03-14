#ifndef GUINATIVEEVENT_H
#define GUINATIVEEVENT_H

#include "GuiAbstractNativeEvent.h"
#include <vector>

#include <QAbstractNativeEventFilter>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

class QMainWindow;
class GUIApplicationNativeEventAware;

namespace Gui
{
class GUIApplicationNativeEventAware;

class GuiNativeEvent: public GuiAbstractNativeEvent
{
    Q_OBJECT
public:
    GuiNativeEvent(GUIApplicationNativeEventAware* app);
    ~GuiNativeEvent() override;
    void initSpaceball(QMainWindow* window) override final;

private:
    GuiNativeEvent();
    GuiNativeEvent(const GuiNativeEvent&);
    GuiNativeEvent& operator=(const GuiNativeEvent&);

public:
    static bool xcbEventFilter(void* message, long* result);
};
}  // namespace Gui

#endif  // GUINATIVEEVENT_H
