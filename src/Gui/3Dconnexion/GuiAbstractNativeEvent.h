#ifndef GUIABSTRACTNATIVEEVENT_H
#define GUIABSTRACTNATIVEEVENT_H

#include <QObject>
#include <vector>

class QMainWindow;

namespace Gui
{
class GUIApplicationNativeEventAware;

class GuiAbstractNativeEvent: public QObject
{
    Q_OBJECT
public:
    explicit GuiAbstractNativeEvent(GUIApplicationNativeEventAware* app);
    ~GuiAbstractNativeEvent() override = 0;
    virtual void initSpaceball(QMainWindow* window) = 0;

private:
    GuiAbstractNativeEvent();
    GuiAbstractNativeEvent(const GuiAbstractNativeEvent&);
    GuiAbstractNativeEvent& operator=(const GuiAbstractNativeEvent&);

protected:
    static GUIApplicationNativeEventAware* mainApp;
    static std::vector<int> motionDataArray;
};
}  // namespace Gui


#endif  // GUIABSTRACTNATIVEEVENT_H
