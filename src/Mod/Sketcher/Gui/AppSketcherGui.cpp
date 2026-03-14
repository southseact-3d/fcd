#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Language/Translator.h>
#include <Gui/WidgetFactory.h>

#include "PropertyConstraintListItem.h"
#include "SketcherSettings.h"
#include "SoZoomTranslation.h"
#include "ViewProviderPython.h"
#include "ViewProviderSketch.h"
#include "ViewProviderSketchGeometryExtension.h"
#include "ViewProviderSketchGeometryExtensionPy.h"
#include "Workbench.h"


// create the commands
void CreateSketcherCommands();
void CreateSketcherCommandsCreateGeo();
void CreateSketcherCommandsConstraints();
void CreateSketcherCommandsConstraintAccel();
void CreateSketcherCommandsAlterGeo();
void CreateSketcherCommandsBSpline();
void CreateSketcherCommandsOverlay();
void CreateSketcherCommandsVirtualSpace();

void loadSketcherResource()
{
    // add resources and reloads the translators
    Q_INIT_RESOURCE(Sketcher);
    Q_INIT_RESOURCE(Sketcher_translation);
    Gui::Translator::instance()->refresh();
}

namespace SketcherGui
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("SketcherGui")
    {
        initialize("This module is the SketcherGui module.");  // register with Python
    }

    ~Module() override
    {}

private:
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace SketcherGui

/* Python entry */
PyMOD_INIT_FUNC(SketcherGui)
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        PyMOD_Return(nullptr);
    }
    try {
        Base::Interpreter().runString("import PartGui");
        Base::Interpreter().runString("import Sketcher");
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }

    PyObject* sketcherGuiModule = SketcherGui::initModule();
    Base::Console().log("Loading GUI of Sketcher module… done\n");

    Gui::BitmapFactory().addPath(QStringLiteral(":/icons/constraints"));
    Gui::BitmapFactory().addPath(QStringLiteral(":/icons/elements"));
    Gui::BitmapFactory().addPath(QStringLiteral(":/icons/general"));
    Gui::BitmapFactory().addPath(QStringLiteral(":/icons/geometry"));
    // Gui::BitmapFactory().addPath(QStringLiteral(":/icons/obsolete"));
    Gui::BitmapFactory().addPath(QStringLiteral(":/icons/pointers"));
    Gui::BitmapFactory().addPath(QStringLiteral(":/icons/splines"));
    Gui::BitmapFactory().addPath(QStringLiteral(":/icons/tools"));
    Gui::BitmapFactory().addPath(QStringLiteral(":/icons/overlay"));

    // instantiating the commands
    CreateSketcherCommands();
    CreateSketcherCommandsCreateGeo();
    CreateSketcherCommandsConstraints();
    CreateSketcherCommandsAlterGeo();
    CreateSketcherCommandsConstraintAccel();
    CreateSketcherCommandsBSpline();
    CreateSketcherCommandsOverlay();
    CreateSketcherCommandsVirtualSpace();

    SketcherGui::Workbench::init();

    // Add Types to module
    Base::Interpreter().addType(
        &SketcherGui::ViewProviderSketchGeometryExtensionPy ::Type,
        sketcherGuiModule,
        "ViewProviderSketchGeometryExtension"
    );

    // init objects
    SketcherGui::ViewProviderSketch ::init();
    SketcherGui::ViewProviderPython ::init();
    SketcherGui::ViewProviderCustom ::init();
    SketcherGui::ViewProviderCustomPython ::init();
    SketcherGui::SoZoomTranslation ::initClass();
    SketcherGui::SoSketchFaces ::initClass();
    SketcherGui::PropertyConstraintListItem ::init();
    SketcherGui::ViewProviderSketchGeometryExtension ::init();

    (void)new Gui::PrefPageProducer<SketcherGui::SketcherSettings>(
        QT_TRANSLATE_NOOP("QObject", "Sketcher")
    );
    (void)new Gui::PrefPageProducer<SketcherGui::SketcherSettingsGrid>(
        QT_TRANSLATE_NOOP("QObject", "Sketcher")
    );
    (void)new Gui::PrefPageProducer<SketcherGui::SketcherSettingsDisplay>(
        QT_TRANSLATE_NOOP("QObject", "Sketcher")
    );
    (void)new Gui::PrefPageProducer<SketcherGui::SketcherSettingsAppearance>(
        QT_TRANSLATE_NOOP("QObject", "Sketcher")
    );

    // add resources and reloads the translators
    loadSketcherResource();

    PyMOD_Return(sketcherGuiModule);
}
