#ifndef BASE_CONSOLEOBSERVER_H
#define BASE_CONSOLEOBSERVER_H

#include <Base/Console.h>
#include <Base/Stream.h>


namespace Base
{

//=========================================================================
// some special observers

/** The LoggingConsoleObserver class
 *  This class is used by the main modules to write Console messages and logs to a file
 */
class BaseExport ConsoleObserverFile: public ILogger
{
public:
    explicit ConsoleObserverFile(const char* sFileName);
    ~ConsoleObserverFile() override;

    void sendLog(
        const std::string& notifiername,
        const std::string& msg,
        LogStyle level,
        IntendedRecipient recipient,
        ContentType content
    ) override;
    const char* name() override
    {
        return "File";
    }

    ConsoleObserverFile(const ConsoleObserverFile&) = delete;
    ConsoleObserverFile(ConsoleObserverFile&&) = delete;
    ConsoleObserverFile& operator=(const ConsoleObserverFile&) = delete;
    ConsoleObserverFile& operator=(ConsoleObserverFile&&) = delete;

private:
    Base::ofstream cFileStream;
};

/** The CmdConsoleObserver class
 *  This class is used by the main modules to write Console messages and logs the system con.
 */
class BaseExport ConsoleObserverStd: public ILogger
{
public:
    ConsoleObserverStd();
    ~ConsoleObserverStd() override;
    void sendLog(
        const std::string& notifiername,
        const std::string& msg,
        LogStyle level,
        IntendedRecipient recipient,
        ContentType content
    ) override;
    const char* name() override
    {
        return "Console";
    }

    ConsoleObserverStd(const ConsoleObserverStd&) = delete;
    ConsoleObserverStd(ConsoleObserverStd&&) = delete;
    ConsoleObserverStd& operator=(const ConsoleObserverStd&) = delete;
    ConsoleObserverStd& operator=(ConsoleObserverStd&&) = delete;

private:
    bool useColorStderr;
    void Warning(const char* sWarn);
    void Message(const char* sMsg);
    void Error(const char* sErr);
    void Log(const char* sLog);
    void Critical(const char* sCritical);
};

/** The ILoggerBlocker class
 *  This class allows one to temporary block then automatically restore arbitrary message types
 *  on a particular console observer.
 */
class BaseExport ILoggerBlocker
{
public:
    // Constructor that will block message types passed as parameter. By default, all types are
    // blocked.
    inline explicit ILoggerBlocker(
        const char* co,
        ConsoleMsgFlags msgTypes = ConsoleSingleton::MsgType_Txt | ConsoleSingleton::MsgType_Log
            | ConsoleSingleton::MsgType_Wrn | ConsoleSingleton::MsgType_Err
            | ConsoleSingleton::MsgType_Critical | ConsoleSingleton::MsgType_Notification
    );
    // Disable copy & move constructors
    ILoggerBlocker(ILoggerBlocker const&) = delete;
    ILoggerBlocker(ILoggerBlocker const&&) = delete;
    // Disable assignment & move-assignment operator
    ILoggerBlocker& operator=(ILoggerBlocker const&) = delete;
    ILoggerBlocker& operator=(ILoggerBlocker const&&) = delete;
    // Destructor that will restore message type settings.
    inline ~ILoggerBlocker();

private:
    ConsoleMsgFlags msgTypesBlocked = 0;  // Stores message types blocked by the blocker
    const char* conObs;                   //  Stores console observer name that blocker acts on
};

ILoggerBlocker::ILoggerBlocker(const char* co, ConsoleMsgFlags msgTypes)
    : conObs(co)
{
    msgTypesBlocked = Console().setEnabledMsgType(conObs, msgTypes, false);
}

ILoggerBlocker::~ILoggerBlocker()
{
    try {
#ifdef FC_DEBUG
        auto debug = Console().setEnabledMsgType(conObs, msgTypesBlocked, true);
        if (debug != msgTypesBlocked) {
            Console().warning("Enabled message types have been changed while ILoggerBlocker was set\n");
        }
#else
        Console().setEnabledMsgType(conObs, msgTypesBlocked, true);
#endif
    }
    catch (...) {
    }
}

class BaseExport RedirectStdOutput: public std::streambuf
{
public:
    RedirectStdOutput();

protected:
    int overflow(int ch = EOF) override;
    int sync() override;

private:
    std::string buffer;
};

class BaseExport RedirectStdError: public std::streambuf
{
public:
    RedirectStdError();

protected:
    int overflow(int ch = EOF) override;
    int sync() override;

private:
    std::string buffer;
};

class BaseExport RedirectStdLog: public std::streambuf
{
public:
    RedirectStdLog();

protected:
    int overflow(int ch = EOF) override;
    int sync() override;

private:
    std::string buffer;
};


}  // namespace Base

#endif  // BASE_CONSOLEOBSERVER_H
