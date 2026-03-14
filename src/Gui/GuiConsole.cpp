#include <FCConfig.h>

#ifdef FC_OS_WIN32
# include "io.h"
# include <windows.h>
#endif
#include <fcntl.h>
#include <iostream>

#include "GuiConsole.h"

using namespace Gui;

#ifdef FC_OS_WIN32

const unsigned int GUIConsole::s_nMaxLines = 1000;
unsigned int GUIConsole::s_nRefCount = 0;

/** Constructor
 *  Open a Top level Window and redirect the
 *  stdin, stdout and stderr stream to it.
 *  Not needed in Linux!
 */
GUIConsole::GUIConsole(void)
{
    if (!s_nRefCount++) {
        bLog = false;

        CONSOLE_SCREEN_BUFFER_INFO csbi;

        ::AllocConsole();

        ::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        csbi.dwSize.Y = s_nMaxLines;
        ::SetConsoleScreenBufferSize(::GetStdHandle(STD_OUTPUT_HANDLE), csbi.dwSize);
        ::SetConsoleTitleA("FreeCAD Console");

        *stdout = *::_fdopen(
            ::_open_osfhandle(reinterpret_cast<intptr_t>(::GetStdHandle(STD_OUTPUT_HANDLE)), _O_TEXT),
            "w"
        );
        ::setvbuf(stdout, 0, _IONBF, 0);

        *stdin = *::_fdopen(
            ::_open_osfhandle(reinterpret_cast<intptr_t>(::GetStdHandle(STD_INPUT_HANDLE)), _O_TEXT),
            "r"
        );
        ::setvbuf(stdin, 0, _IONBF, 0);

        *stderr = *::_fdopen(
            ::_open_osfhandle(reinterpret_cast<intptr_t>(::GetStdHandle(STD_ERROR_HANDLE)), _O_TEXT),
            "w"
        );
        ::setvbuf(stderr, 0, _IONBF, 0);
    }
}

/** Destructor
 *  Close the window and redirect the streams back
 */
GUIConsole::~GUIConsole(void)
{
    if (!--s_nRefCount) {
        FreeConsole();
    }
}

void GUIConsole::sendLog(
    const std::string& notifiername,
    const std::string& msg,
    Base::LogStyle level,
    Base::IntendedRecipient recipient,
    Base::ContentType content
)
{
    (void)notifiername;

    // Do not log translated messages, or messages intended only to the user to std log
    if (recipient == Base::IntendedRecipient::User || content == Base::ContentType::Translated) {
        return;
    }

    int color = -1;
    switch (level) {
        case Base::LogStyle::Warning:
            color = FOREGROUND_RED | FOREGROUND_GREEN;
            break;
        case Base::LogStyle::Message:
            color = FOREGROUND_GREEN;
            break;
        case Base::LogStyle::Error:
            color = FOREGROUND_RED;
            break;
        case Base::LogStyle::Log:
            color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            break;
        case Base::LogStyle::Critical:
            color = FOREGROUND_RED | FOREGROUND_GREEN;
            break;
        default:
            break;
    }

    ::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), color);
    printf("%s", msg.c_str());
    ::SetConsoleTextAttribute(
        ::GetStdHandle(STD_OUTPUT_HANDLE),
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
    );
}

#else /* FC_OS_LINUX */

// safely ignore GUIConsole::s_nMaxLines and  GUIConsole::s_nRefCount
GUIConsole::GUIConsole() = default;
GUIConsole::~GUIConsole() = default;
void GUIConsole::sendLog(
    const std::string& notifiername,
    const std::string& msg,
    Base::LogStyle level,
    Base::IntendedRecipient recipient,
    Base::ContentType content
)
{
    (void)notifiername;

    // Do not log translated messages, or messages intended only to the user to std log
    if (recipient == Base::IntendedRecipient::User || content == Base::ContentType::Translated) {
        return;
    }

    switch (level) {
        case Base::LogStyle::Warning:
            std::cerr << "Warning: " << msg;
            break;
        case Base::LogStyle::Message:
            std::cout << msg;
            break;
        case Base::LogStyle::Error:
            std::cerr << "Error: " << msg;
            break;
        case Base::LogStyle::Log:
            std::clog << msg;
            break;
        case Base::LogStyle::Critical:
            std::cout << "Critical: " << msg;
            break;
        default:
            break;
    }
}

#endif /* FC_OS_LINUX */
