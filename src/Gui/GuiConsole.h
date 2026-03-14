#ifndef GUI_GUICONSOLE_H
#define GUI_GUICONSOLE_H


#include <Base/Console.h>

namespace Gui
{

/** The console window class
 *  This class opens a console window when instantiated
 *  and redirects the stdio streams to it as long it exists.
 *  This is for Windows only!
 *  After instantiation it automatically registers itself at
 *  the FCConsole class and gets all the FCConsoleObserver
 *  messages. The class must not used directly! Only the
 *  FCConsole class is allowed!
 *  @see FCConsole
 *  \author Jürgen Riegel
 */
class GuiExport GUIConsole: public Base::ILogger
{
public:
    /// Constructor
    GUIConsole();
    /// Destructor
    ~GUIConsole() override;
    void sendLog(
        const std::string& notifiername,
        const std::string& msg,
        Base::LogStyle level,
        Base::IntendedRecipient recipient,
        Base::ContentType content
    ) override;
    const char* name() override
    {
        return "GUIConsole";
    }

protected:
    static const unsigned int s_nMaxLines;
    static unsigned int s_nRefCount;
};

}  // namespace Gui

#endif  // GUI_GUICONSOLE_H
