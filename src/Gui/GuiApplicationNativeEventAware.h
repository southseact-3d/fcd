#ifndef GUIAPPLICATIONNATIVEEVENTAWARE_H
#define GUIAPPLICATIONNATIVEEVENTAWARE_H

#include <QApplication>
#include <vector>


class QMainWindow;

namespace Gui
{
#if defined(_USE_3DCONNEXION_SDK) || defined(SPNAV_FOUND)
class GuiNativeEvent;
#endif  // Spacemice
class GUIApplicationNativeEventAware: public QApplication
{
    Q_OBJECT
public:
    GUIApplicationNativeEventAware(int& argc, char* argv[]);
    ~GUIApplicationNativeEventAware() override;
    void initSpaceball(QMainWindow* window);
    bool isSpaceballPresent() const
    {
        return spaceballPresent;
    }
    void setSpaceballPresent(bool present)
    {
        spaceballPresent = present;
    }
    bool processSpaceballEvent(QObject* object, QEvent* event);
    void postMotionEvent(std::vector<int> motionDataArray);
    void postButtonEvent(int buttonNumber, int buttonPress);

private:
    bool spaceballPresent;
    void importSettings(std::vector<int>& motionDataArray);
    float convertPrefToSensitivity(int value);
#if defined(_USE_3DCONNEXION_SDK) || defined(SPNAV_FOUND)
    GuiNativeEvent* nativeEvent;
#endif
};  // end class GUIApplicationNativeEventAware
}  // end namespace Gui

#endif  // GUIAPPLICATIONNATIVEEVENTAWARE_H
