#ifndef GUINATIVEEVENT_H
#define GUINATIVEEVENT_H

#include "GuiAbstractNativeEvent.h"

class QMainWindow;

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
private Q_SLOTS:
    void pollSpacenav();
};
}  // namespace Gui

#endif  // GUINATIVEEVENT_H
