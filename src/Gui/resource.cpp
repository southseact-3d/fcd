#include "BitmapFactory.h"
#include "WidgetFactory.h"
#include "Workbench.h"

// INCLUDE YOUR PREFERENCE PAGES HERE
//
#include "Dialogs/DlgPreferencesImp.h"
#include "PreferencePages/DlgSettings3DViewImp.h"
#include "PreferencePages/DlgSettingsCacheDirectory.h"
#include "PreferencePages/DlgSettingsDocumentImp.h"
#include "PreferencePages/DlgSettingsEditor.h"
#include "PreferencePages/DlgSettingsGeneral.h"
#include "PreferencePages/DlgSettingsMacroImp.h"
#include "PreferencePages/DlgSettingsLightSources.h"
#include "PreferencePages/DlgSettingsNavigation.h"
#include "PreferencePages/DlgSettingsNotificationArea.h"
#include "PreferencePages/DlgSettingsPythonConsole.h"
#include "PreferencePages/DlgSettingsReportView.h"
#include "PreferencePages/DlgSettingsSelection.h"
#include "PreferencePages/DlgSettingsUI.h"
#include "PreferencePages/DlgSettingsViewColor.h"
#include "PreferencePages/DlgSettingsWorkbenchesImp.h"
#include "PreferencePages/DlgSettingsAdvanced.h"
#include "PreferencePages/DlgSettingsPDF.h"

#include "Dialogs/DlgToolbarsImp.h"
#include "Dialogs/DlgActionsImp.h"
#include "Dialogs/DlgKeyboardImp.h"

#if defined(_USE_3DCONNEXION_SDK) || defined(SPNAV_FOUND)
# include "Dialogs/DlgCustomizeSpaceball.h"
# include "Dialogs/DlgCustomizeSpNavSettings.h"
#endif

#include "InputField.h"
#include "QuantitySpinBox.h"
#include "PrefWidgets.h"
#include "ToolBarManager.h"

using namespace Gui;
using namespace Gui::Dialog;

// clang-format off
/**
 * Registers all preference pages or widgets to create them dynamically at any later time.
 */
WidgetFactorySupplier::WidgetFactorySupplier()
{
    // ADD YOUR PREFERENCE PAGES HERE
    //
    //
    new PrefPageProducer<DlgSettingsGeneral>          ( QT_TRANSLATE_NOOP("QObject","General") );
    DlgSettingsGeneral::attachObserver();
    new PrefPageProducer<DlgSettingsDocumentImp>      ( QT_TRANSLATE_NOOP("QObject","General") );
    new PrefPageProducer<DlgSettingsSelection>        ( QT_TRANSLATE_NOOP("QObject","General") );
    new PrefPageProducer<DlgSettingsCacheDirectory>   ( QT_TRANSLATE_NOOP("QObject","General") );
    new PrefPageProducer<DlgSettingsNotificationArea> ( QT_TRANSLATE_NOOP("QObject","General") );
    new PrefPageProducer<DlgSettingsReportView>       ( QT_TRANSLATE_NOOP("QObject","General") );
    new PrefPageProducer<DlgSettings3DViewImp>        ( QT_TRANSLATE_NOOP("QObject","Display") );
    new PrefPageProducer<DlgSettingsLightSources>     ( QT_TRANSLATE_NOOP("QObject","Display") );
    new PrefPageProducer<DlgSettingsUI>               ( QT_TRANSLATE_NOOP("QObject","Display") );
    new PrefPageProducer<DlgSettingsNavigation>       ( QT_TRANSLATE_NOOP("QObject","Display") );
    new PrefPageProducer<DlgSettingsViewColor>        ( QT_TRANSLATE_NOOP("QObject","Display") );
    new PrefPageProducer<DlgSettingsAdvanced>         ( QT_TRANSLATE_NOOP("QObject","Display") );
    new PrefPageProducer<DlgSettingsWorkbenchesImp>   ( QT_TRANSLATE_NOOP("QObject","Workbenches") );
    new PrefPageProducer<DlgSettingsPDF>              ( QT_TRANSLATE_NOOP("QObject","Import-Export") );
    new PrefPageProducer<DlgSettingsMacroImp>         ( QT_TRANSLATE_NOOP("QObject", "Python"));
    new PrefPageProducer<DlgSettingsPythonConsole>    ( QT_TRANSLATE_NOOP("QObject", "Python"));
    new PrefPageProducer<DlgSettingsEditor>           ( QT_TRANSLATE_NOOP("QObject", "Python"));

    // ADD YOUR CUSTOMIZE PAGES HERE
    //
    //
    new CustomPageProducer<DlgCustomKeyboardImp>;
    new CustomPageProducer<DlgCustomToolbarsImp>;
    new CustomPageProducer<DlgCustomActionsImp>;
#if defined(_USE_3DCONNEXION_SDK) || defined(SPNAV_FOUND)
    new CustomPageProducer<DlgCustomizeSpNavSettings>;
    new CustomPageProducer<DlgCustomizeSpaceball>;
#endif
    // ADD YOUR PREFERENCE WIDGETS HERE
    //
    //
    new WidgetProducer<Gui::InputField>;
    new WidgetProducer<Gui::PrefSpinBox>;
    new WidgetProducer<Gui::PrefDoubleSpinBox>;
    new WidgetProducer<Gui::PrefLineEdit>;
    new WidgetProducer<Gui::PrefTextEdit>;
    new WidgetProducer<Gui::PrefComboBox>;
    new WidgetProducer<Gui::PrefFontBox>;
    new WidgetProducer<Gui::PrefCheckBox>;
    new WidgetProducer<Gui::PrefCheckableGroupBox>;
    new WidgetProducer<Gui::PrefRadioButton>;
    new WidgetProducer<Gui::PrefSlider>;
    new WidgetProducer<Gui::PrefFileChooser>;
    new WidgetProducer<Gui::PrefColorButton>;
    new WidgetProducer<Gui::PrefUnitSpinBox>;
    new WidgetProducer<Gui::PrefQuantitySpinBox>;
    new WidgetProducer<Gui::CommandIconView>;
    new WidgetProducer<Gui::AccelLineEdit>;
    new WidgetProducer<Gui::ActionSelector>;
    new WidgetProducer<Gui::ColorButton>;
    new WidgetProducer<Gui::UrlLabel>;
    new WidgetProducer<Gui::StatefulLabel>;
    new WidgetProducer<Gui::FileChooser>;
    new WidgetProducer<Gui::UIntSpinBox>;
    new WidgetProducer<Gui::IntSpinBox>;
    new WidgetProducer<Gui::DoubleSpinBox>;
    new WidgetProducer<Gui::QuantitySpinBox>;
    new WidgetProducer<Gui::ExpLineEdit>;
}
// clang-format on
