#ifndef GUI_WAIT_CURSOR_H
#define GUI_WAIT_CURSOR_H

#include <FCGlobal.h>

namespace Gui
{

/**
 * This class sets a waitcursor automatically while a slow operation is running.
 * Therefore you just have to create an instance of WaitCursor before the time
 * consuming operation starts.
 *
 * \code:
 * WaitCursor ac;
 * ...
 * ...                   // slow operation
 * ...
 * \endcode
 *
 * Sometimes you have two slow operations with some user interactions in between them.
 * Avoiding to show the waiting cursor then you have to call the methods @ref restoreCursor()
 * and setWaitCursor manually, like:
 *
 * \code:
 * WaitCursor ac;
 * ...
 * ...                   // 1st slow operation
 * ac.restoreCursor();
 * ...
 * ...                  // some dialog stuff
 * ac.setWaitCursor();
 * ...
 * ...                  // 2nd slow operation
 * \endcode
 *
 * @author Werner Mayer
 */
class GuiExport WaitCursor
{
public:
    enum FilterEventsFlag
    {
        NoEvents = 0x00,
        KeyEvents = 0x01,
        MouseEvents = 0x02,
        AllEvents = KeyEvents | MouseEvents
    };
    Q_DECLARE_FLAGS(FilterEventsFlags, FilterEventsFlag)

    WaitCursor();
    ~WaitCursor();

    void setWaitCursor();
    void restoreCursor();
    FilterEventsFlags ignoreEvents() const;
    void setIgnoreEvents(FilterEventsFlags flags = AllEvents);

    /**
     * @brief Suspends the wait cursor state by restoring the normal cursor
     * and removing the event filter. To be used before showing an interactive
     * dialog during a long operation.
     */
    static void suspend();

    /**
     * @brief Resumes the wait cursor state by setting the wait cursor
     * and reinstalling the event filter, if a WaitCursor is active.
     */
    static void resume();

private:
    FilterEventsFlags filter;
    static int instances;
};

}  // namespace Gui

#endif  // GUI_WAIT_CURSOR_H
