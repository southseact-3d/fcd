#ifndef SKETCHERGUI_SKETCHERSETTINGS_H
#define SKETCHERGUI_SKETCHERSETTINGS_H

#include <Gui/PropertyPage.h>
#include <memory>


namespace SketcherGui
{
class Ui_SketcherSettings;
class Ui_SketcherSettingsGrid;
class Ui_SketcherSettingsDisplay;
class Ui_SketcherSettingsAppearance;
class SketcherGeneralWidget;
/**
 * The SketcherSettings class implements a preference page to change sketcher settings.
 * @author Werner Mayer
 */
class SketcherSettings: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit SketcherSettings(QWidget* parent = nullptr);
    ~SketcherSettings() override;

    void saveSettings() override;
    void loadSettings() override;

    void resetSettingsToDefaults() override;

protected:
    void changeEvent(QEvent* e) override;
    void dimensioningModeChanged(int index);
    void checkForRestart();

private:
    std::unique_ptr<Ui_SketcherSettings> ui;
};

/**
 * The SketcherSettings class implements a preference page to change sketcher grid settings.
 */
class SketcherSettingsGrid: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit SketcherSettingsGrid(QWidget* parent = nullptr);
    ~SketcherSettingsGrid() override;

    void saveSettings() override;
    void loadSettings() override;

protected:
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_SketcherSettingsGrid> ui;
};

/**
 * The SketcherSettings class implements a preference page to change sketcher display settings.
 * @author Werner Mayer
 */
class SketcherSettingsDisplay: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit SketcherSettingsDisplay(QWidget* parent = nullptr);
    ~SketcherSettingsDisplay() override;

    void saveSettings() override;
    void loadSettings() override;

protected:
    void changeEvent(QEvent* e) override;

private Q_SLOTS:
    void onBtnTVApplyClicked(bool);

private:
    std::unique_ptr<Ui_SketcherSettingsDisplay> ui;
};

/**
 * The SketcherSettings class implements a preference page to change sketcher settings.
 * @author Werner Mayer
 */
class SketcherSettingsAppearance: public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    explicit SketcherSettingsAppearance(QWidget* parent = nullptr);
    ~SketcherSettingsAppearance() override;

    void saveSettings() override;
    void loadSettings() override;

protected:
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_SketcherSettingsAppearance> ui;
};

// Mode for the sketch autoscale feature which scales
// the geometry and zooms the camera when the first
// scale defining constraint is set
enum class AutoScaleMode : int
{
    Always = 0,
    Never = 1,

    // Attempts to find scale reference objects int the viewport
    // (such as a 3d body) and disable the feature if it finds one
    WhenNoScaleFeatureIsVisible = 2
};

}  // namespace SketcherGui

#endif  // SKETCHERGUI_SKETCHERSETTINGS_H
