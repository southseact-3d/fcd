#ifndef GUI_DIALOG_DOWNLOADITEM_H
#define GUI_DIALOG_DOWNLOADITEM_H

#include <QBasicTimer>
#include <QElapsedTimer>
#include <QFile>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTableView>
#include <QTime>
#include <QUrl>


class AutoSaver;
class QFileIconProvider;

class EditTableView: public QTableView
{
    Q_OBJECT

public:
    explicit EditTableView(QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent* event) override;

public Q_SLOTS:
    void removeOne();
    void removeAll();
};

class SqueezeLabel: public QLabel
{
    Q_OBJECT

public:
    explicit SqueezeLabel(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};

/*
    This class will call the save() slot on the parent object when the parent changes.
    It will wait several seconds after changed() to combining multiple changes and
    prevent continuous writing to disk.
  */
class AutoSaver: public QObject
{

    Q_OBJECT

public:
    explicit AutoSaver(QObject* parent);
    ~AutoSaver() override;
    void saveIfNecessary();

public Q_SLOTS:
    void changeOccurred();

protected:
    void timerEvent(QTimerEvent* event) override;

private:
    QBasicTimer m_timer;
    QElapsedTimer m_firstChange;
};

class NetworkAccessManager: public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit NetworkAccessManager(QObject* parent = nullptr);

private Q_SLOTS:
    void authenticationRequired(QNetworkReply* reply, QAuthenticator* auth);  // clazy:exclude=overridden-signal
    void proxyAuthenticationRequired(
        const QNetworkProxy& proxy,
        QAuthenticator* auth
    );  // clazy:exclude=overridden-signal
};

#include "ui_DownloadItem.h"

namespace Gui
{
namespace Dialog
{
class DownloadModel;

class DownloadItem: public QWidget, public Ui_DownloadItem
{
    Q_OBJECT

Q_SIGNALS:
    void statusChanged();

public:
    explicit DownloadItem(
        QNetworkReply* reply = nullptr,
        bool requestFileName = false,
        QWidget* parent = nullptr
    );
    bool downloading() const;
    bool downloadedSuccessfully() const;

    QUrl m_url;
    QString m_fileName;

    QFile m_output;
    QNetworkReply* m_reply;

private Q_SLOTS:
    void stop();
    void tryAgain();
    void open();
    void openFolder();

    void downloadReadyRead();
    void error(QNetworkReply::NetworkError code);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void metaDataChanged();
    void finished();

private:
    void contextMenuEvent(QContextMenuEvent*) override;
    void getFileName();
    void init();
    void updateInfoLabel();
    QString dataString(int size) const;
    QString getDownloadDirectory() const;
    QString saveFileName(const QString& directory) const;

    bool m_requestFileName;
    qint64 m_bytesReceived;
    QElapsedTimer m_downloadTime;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DOWNLOADITEM_H
