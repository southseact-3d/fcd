#ifndef SketcherGui_DrawSketchKeyboardManager_H
#define SketcherGui_DrawSketchKeyboardManager_H


#include <QEvent>
#include <QKeyEvent>

#include <QTimer>

#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>


namespace Gui
{
class ViewProvider;
}  // namespace Gui

namespace SketcherGui
{

class ViewProviderSketch;

/** Class implementing an event filter for DrawSketchHandler tools, enabling seamless introduction
 * of values to parameters, including units, while still allowing operation of shortcuts.
 *
 * The basic mechanism to decide which control should respond is based on using timers, type of
 * entered event.
 */
class DrawSketchKeyboardManager: public QObject
{
    Q_OBJECT

public:
    DrawSketchKeyboardManager();


    /** Indicates whether the DSH control (e.g. on-view parameter or widget) should handle keyboard
     * input or should signal it via boost */
    enum class KeyboardEventHandlingMode
    {
        DSHControl,
        ViewProvider
    };

    /// returns whether the provided entity will currently receive the event.
    bool isMode(KeyboardEventHandlingMode mode);

    /// returns which entity will currently receive the event.
    KeyboardEventHandlingMode getMode();

    bool eventFilter(QObject* object, QEvent* event);

    /// sets the timeout to the amount of milliseconds.
    void setTimeOut(int milliseconds);

    // returns the current timeout amount
    int timeOut();

private:
    /// This function decides whether events should be send to the ViewProvider
    /// or to the UI control of DSH.
    void detectKeyboardEventHandlingMode(QKeyEvent* keyEvent);

    void onTimeOut();

private:
    /// Viewer responsible for the active document
    Gui::View3DInventorViewer* vpViewer = nullptr;
    KeyboardEventHandlingMode keyMode;

    QTimer timer;

    int timeOutValue = 2000;
};

}  // namespace SketcherGui

#endif  // SketcherGui_DrawSketchKeyboardManager_H
