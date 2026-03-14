#ifndef GUI_DLG_ABOUT_H
#define GUI_DLG_ABOUT_H

#include <QDialog>
#include <QTextBrowser>
#include <Gui/MDIView.h>
#include <qlocale.h>

namespace Gui
{
namespace Dialog
{

class Ui_AboutApplication;

class GuiExport AboutDialogFactory
{
public:
    AboutDialogFactory() = default;
    virtual ~AboutDialogFactory();

    virtual QDialog* create(QWidget* parent) const;

    static const AboutDialogFactory* defaultFactory();
    static void setDefaultFactory(AboutDialogFactory* factory);

private:
    static AboutDialogFactory* factory;
};

class GuiExport LicenseView: public Gui::MDIView
{
    Q_OBJECT

public:
    explicit LicenseView(QWidget* parent = nullptr);
    ~LicenseView() override;

    void setSource(const QUrl& url);
    const char* getName() const override
    {
        return "LicenseView";
    }

private:
    QTextBrowser* browser;
};

/** This widget provides the "About dialog" of an application.
 * This shows the current version, the build number and date.
 * \author Werner Mayer
 */
class GuiExport AboutDialog: public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget* parent = nullptr);
    ~AboutDialog() override;

protected:
    void setupLabels();
    void showCredits();
    void showLicenseInformation();
    QString getAdditionalLicenseInformation() const;
    void showLibraryInformation();
    void showCollectionInformation();
    void showPrivacyPolicy();
    void showOrHideImage(const QRect& rect);
    void addModuleInfo(QTextStream& inout_str, const QString& modPath, bool& inout_first);

protected:
    QPixmap aboutImage() const;
    virtual void copyToClipboard();
    void linkActivated(const QUrl& link);

private:
    Ui_AboutApplication* ui;
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DLG_ABOUT_H
