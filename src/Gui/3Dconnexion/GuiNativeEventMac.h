#ifndef GUINATIVEEVENT_H
#define GUINATIVEEVENT_H

#include "GuiAbstractNativeEvent.h"

class QMainWindow;

#include <IOKit/IOKitLib.h>
#include <ConnexionClientAPI.h>
extern int16_t SetConnexionHandlers(
    ConnexionMessageHandlerProc messageHandler,
    ConnexionAddedHandlerProc addedHandler,
    ConnexionRemovedHandlerProc removedHandler,
    bool useSeparateThread
) __attribute__((weak_import));
// Note that InstallConnexionHandlers will be replaced with
// SetConnexionHandlers "in the future".
extern OSErr InstallConnexionHandlers(
    ConnexionMessageHandlerProc messageHandler,
    ConnexionAddedHandlerProc addedHandler,
    ConnexionRemovedHandlerProc removedHandler
) __attribute__((weak_import));
extern UInt16 RegisterConnexionClient(UInt32 signature, UInt8* name, UInt16 mode, UInt32 mask)
    __attribute__((weak_import));
extern void SetConnexionClientMask(uint16_t clientID, uint32_t mask) __attribute__((weak_import));
extern void SetConnexionClientButtonMask(uint16_t clientID, uint32_t buttonMask)
    __attribute__((weak_import));
extern void UnregisterConnexionClient(UInt16 clientID) __attribute__((weak_import));
extern void CleanupConnexionHandlers(void) __attribute__((weak_import));

extern int16_t ConnexionControl(uint32_t message, int32_t param, int32_t* result);
extern int16_t ConnexionClientControl(uint16_t clientID, uint32_t message, int32_t param, int32_t* result);
extern int16_t ConnexionGetCurrentDevicePrefs(uint32_t deviceID, ConnexionDevicePrefs* prefs);
extern int16_t ConnexionSetButtonLabels(uint8_t* labels, uint16_t size);

namespace Gui
{
class GUIApplicationNativeEventAware;

class GuiNativeEvent: public GuiAbstractNativeEvent
{
    Q_OBJECT
public:
    explicit GuiNativeEvent(GUIApplicationNativeEventAware* app);
    ~GuiNativeEvent() override;
    void initSpaceball(QMainWindow* window) final;

private:
    GuiNativeEvent();
    GuiNativeEvent(const GuiNativeEvent&);
    GuiNativeEvent& operator=(const GuiNativeEvent&);

private:
    static UInt16 tdxClientID; /* ID assigned by the driver */
    static uint32_t lastButtons;
    static void tdx_drv_handler(io_connect_t connection, natural_t messageType, void* messageArgument);
};
}  // namespace Gui

#endif  // GUINATIVEEVENT_H
