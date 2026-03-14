#ifndef GUI_DIALOG_DOCUMENTRECOVERY_H
#define GUI_DIALOG_DOCUMENTRECOVERY_H

#include <QDialog>
#include <QFileInfo>
#include <QFileInfoList>
#include <QList>
#include <QScopedPointer>


namespace Gui
{
namespace Dialog
{

class DocumentRecoveryPrivate;

/*!
 @author Werner Mayer
 */
class DocumentRecovery: public QDialog
{
    Q_OBJECT

public:
    explicit DocumentRecovery(const QList<QFileInfo>&, QWidget* parent = nullptr);
    ~DocumentRecovery() override;

    void accept() override;
    bool foundDocuments() const;

protected:
    void closeEvent(QCloseEvent*) override;
    void contextMenuEvent(QContextMenuEvent*) override;
    QString createProjectFile(const QString&);
    void cleanup(QDir&, const QList<QFileInfo>&, const QString&);

protected:
    void onButtonCleanupClicked();
    void onDeleteSection();

private:
    static std::string doctools;
    QScopedPointer<DocumentRecoveryPrivate> d_ptr;
    Q_DISABLE_COPY(DocumentRecovery)
    Q_DECLARE_PRIVATE(DocumentRecovery)
};

class DocumentRecoveryFinder
{
public:
    bool checkForPreviousCrashes();

private:
    void checkDocumentDirs(QDir&, const QList<QFileInfo>&, const QString&);
    bool showRecoveryDialogIfNeeded();

private:
    QList<QFileInfo> restoreDocFiles;
};

class DocumentRecoveryHandler
{
public:
    void checkForPreviousCrashes(
        const std::function<void(QDir&, const QList<QFileInfo>&, const QString&)>& callableFunc
    ) const;
};

class DocumentRecoveryCleaner
{
public:
    void clearDirectory(const QFileInfo& dir);
    void setIgnoreFiles(const QStringList&);
    void setIgnoreDirectories(const QFileInfoList&);

private:
    void subtractFiles(QStringList&);
    void subtractDirs(QFileInfoList&);

private:
    QStringList ignoreFiles;
    QFileInfoList ignoreDirs;
};

}  // namespace Dialog

}  // namespace Gui


#endif  // GUI_DIALOG_DOCUMENTRECOVERY_H
