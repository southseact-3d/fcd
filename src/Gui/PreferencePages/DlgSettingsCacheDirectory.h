#ifndef GUI_DIALOG_DLGSETTINGSCACHEDIRECTORY_H
#define GUI_DIALOG_DLGSETTINGSCACHEDIRECTORY_H

#include <Gui/PropertyPage.h>
#include <memory>

namespace Gui
{
namespace Dialog
{
class Ui_DlgSettingsCacheDirectory;

/**
 * The DlgSettingsCacheDirectory class implements a preference page to change settings
 * for the cache directory handling.
 * @author Werner Mayer
 */
class DlgSettingsCacheDirectory: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettingsCacheDirectory(QWidget* parent = nullptr);
    ~DlgSettingsCacheDirectory() override;

    void saveSettings() override;
    void loadSettings() override;
    void resetSettingsToDefaults() override;

protected:
    void changeEvent(QEvent* e) override;

private:
    void runCheck();
    void openDirectory();
    void setCurrentCacheSize(const QString&);

private:
    static QString currentSize;
    std::unique_ptr<Ui_DlgSettingsCacheDirectory> ui;
};

class ApplicationCache: public QObject
{
    Q_OBJECT

public:
    enum class Period
    {
        Always,
        Daily,
        Weekly,
        Monthly,
        Yearly,
        Never
    };

    ApplicationCache();
    void applyUserSettings();
    void setPeriod(Period);
    void setLimit(qint64);
    bool periodicCheckOfSize() const;
    qint64 size() const;
    bool performAction(qint64);

    static QString toString(qint64 size);
    static qint64 toBytes(unsigned int);

private:
    void clearDirectory(const QString& path);
    qint64 dirSize(QString dirPath) const;

private:
    qint64 limit;
    int numDays;
};

class ApplicationCacheSettings
{
public:
    static unsigned int getCacheSizeLimit();
    static void setCacheSizeLimit(unsigned int);
    static int getCheckPeriod();
    static void setCheckPeriod(int);
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGSCACHEDIRECTORY_H
