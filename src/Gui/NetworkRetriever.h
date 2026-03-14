#ifndef GUI_NETWORKRETRIEVER_H
#define GUI_NETWORKRETRIEVER_H

#include <QProcess>

#include "Command.h"


namespace Gui
{

/**
 * The NetworkRetriever class encapsulates the GNU tool \a wget.
 * wget is a non-interactive network retriever to download e.g. the whole
 * file structure from a server.
 * \author Werner Mayer
 */
class NetworkRetriever: public QObject
{
    Q_OBJECT

public:
    NetworkRetriever(QObject* parent = nullptr);
    ~NetworkRetriever() override;

    void setNumberOfTries(int);
    void setOutputFile(const QString&);
    void setEnableTimestamp(bool);
    void setProxy(const QString&, const QString& = QString(), const QString& = QString());
    void setEnableRecursive(bool, int = 0);
    void setFollowRelative(bool);
    void setEnableConvert(bool);
    void setFetchImages(bool);
    void setEnableHTMLExtension(bool);
    void setNoParent(bool);

    void setOutputDirectory(const QString&);
    bool startDownload(const QString&);
    bool isDownloading() const;
    void abort();

    static bool testWget();

Q_SIGNALS:
    void wgetExited();

private Q_SLOTS:
    void testFailure();
    void wgetFinished(int, QProcess::ExitStatus);

private:
    QProcess* wget;
    struct NetworkRetrieverP* d;
};

// --------------------------------------------------------------------

class StdCmdDownloadOnlineHelp: public QObject, public Command
{
    Q_OBJECT

public:
    StdCmdDownloadOnlineHelp(QObject* parent = nullptr);
    ~StdCmdDownloadOnlineHelp() override;
    /** i18n stuff of the command. */
    void languageChange() override;

protected:
    void activated(int iMsg) override;

    /** Creates the action object. */
    Action* createAction() override;

private Q_SLOTS:
    void wgetFinished();

private:
    NetworkRetriever* wget;
};

}  // namespace Gui

#endif  // GUI_NETWORKRETRIEVER_H
