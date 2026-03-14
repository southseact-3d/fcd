#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QProcess>
#include <QTextStream>

#include <App/Application.h>
#include <Base/Console.h>
#include "Assistant.h"

using namespace Gui;

/* TRANSLATOR Gui::Assistant */

Assistant::Assistant()
    : proc(nullptr)
{}

Assistant::~Assistant()
{
    if (proc && proc->state() == QProcess::Running) {
        proc->terminate();
        proc->waitForFinished(3000);
    }
}

void Assistant::showDocumentation(const QString& page)
{
    if (!startAssistant()) {
        return;
    }
    if (!page.isEmpty()) {
        QTextStream str(proc);
        str << QLatin1String("setSource qthelp://org.freecad.usermanual/doc/") << page
            << QLatin1String("\n\n");
    }
}

bool Assistant::startAssistant()
{
    if (!proc) {
        proc = new QProcess();
        connect(proc, &QProcess::readyReadStandardOutput, this, &Assistant::readyReadStandardOutput);
        connect(proc, &QProcess::readyReadStandardError, this, &Assistant::readyReadStandardError);
    }

    if (proc->state() != QProcess::Running) {
#ifdef Q_OS_WIN
        QString app;
        app = QDir::toNativeSeparators(
            QString::fromStdString(App::Application::getHomePath()) + QLatin1String("bin/")
        );
#elif defined(Q_OS_MACOS)
        QString app = QCoreApplication::applicationDirPath() + QDir::separator();
#else
# if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QString app = QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();
# else
        QString app = QLibraryInfo::path(QLibraryInfo::BinariesPath) + QDir::separator();
# endif
#endif
        app += QLatin1String("assistant");

        // get the name of the executable and the doc path
        QString exe = QString::fromStdString(App::Application::getExecutableName());
        QString doc = QString::fromStdString(App::Application::getHelpDir());
        QString qhc = doc + exe.toLower() + QLatin1String(".qhc");


        QFileInfo fi(qhc);
        if (!fi.isReadable()) {
            QMessageBox::critical(
                nullptr,
                tr("%1 Help").arg(exe),
                tr("%1 help files not found (%2). You might need to install the %1 documentation "
                   "package.")
                    .arg(exe, qhc)
            );
            return false;
        }

        static bool first = true;
        if (first) {
            Base::Console().log("Help file at %s\n", (const char*)qhc.toUtf8());
            first = false;
        }

        // AppImage start
        // AppImage mount location changes on each start. Assistant caches freecad.qhc
        // file and sets an absolute path. As a result embedded documentation only works
        // on the first AppImage (help) run. Register the .gch file, to overcome the issue.
        static bool start = true;
        if (start) {
            char* appimage = getenv("APPIMAGE");
            if (appimage) {
                QString qch = doc + exe.toLower() + QLatin1String(".qch");
                QFileInfo fi(qch);
                if (fi.isReadable()) {
                    // Assume documentation is embedded
                    // Unregister qch file (path) from previous AppImage run
                    QStringList args;

                    args << QLatin1String("-collectionFile") << qhc << QLatin1String("-unregister")
                         << qch;

                    proc->start(app, args);

                    if (!proc->waitForFinished(50000)) {
                        QMessageBox::critical(
                            nullptr,
                            tr("%1 Help").arg(exe),
                            tr("Unable to launch Qt Assistant (%1)").arg(app)
                        );
                        return false;
                    }

                    // Register qch file (path) for current AppImage run
                    args.clear();

                    args << QLatin1String("-collectionFile") << qhc << QLatin1String("-register")
                         << qch;

                    proc->start(app, args);

                    if (!proc->waitForFinished(50000)) {
                        QMessageBox::critical(
                            nullptr,
                            tr("%1 Help").arg(exe),
                            tr("Unable to launch Qt Assistant (%1)").arg(app)
                        );
                        return false;
                    }
                }
            }
            start = false;
        }
        // AppImage end

        QStringList args;

        args << QLatin1String("-collectionFile") << qhc << QLatin1String("-enableRemoteControl");

        proc->start(app, args);

        if (!proc->waitForStarted()) {
            QMessageBox::critical(
                nullptr,
                tr("%1 Help").arg(exe),
                tr("Unable to launch Qt Assistant (%1)").arg(app)
            );
            return false;
        }
    }

    return true;
}

void Assistant::readyReadStandardOutput()
{
    QByteArray data = proc->readAllStandardOutput();
    Base::Console().log("Help view: %s\n", data.constData());
}

void Assistant::readyReadStandardError()
{
    QByteArray data = proc->readAllStandardError();
    Base::Console().log("Help view: %s\n", data.constData());
}

#include "moc_Assistant.cpp"
