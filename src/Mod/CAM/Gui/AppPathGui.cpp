#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>
#include <Gui/Application.h>
#include <Gui/WidgetFactory.h>
#include <Gui/Language/Translator.h>

#include "DlgSettingsPathColor.h"
#include "ViewProviderArea.h"
#include "ViewProviderPath.h"
#include "ViewProviderPathCompound.h"
#include "ViewProviderPathShape.h"


// use a different name to CreateCommand()
void CreatePathCommands();

void loadPathResource()
{
    // add resources and reloads the translators
    Q_INIT_RESOURCE(Path);
    Q_INIT_RESOURCE(Path_translation);
    Gui::Translator::instance()->refresh();
}

namespace PathGui
{
extern PyObject* initModule();
}

/* Python entry */
PyMOD_INIT_FUNC(PathGui)
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        PyMOD_Return(nullptr);
    }
    try {
        Base::Interpreter().runString("import PartGui");
        Base::Interpreter().runString("import Path");
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }
    PyObject* mod = PathGui::initModule();
    Base::Console().log("Loading GUI of Path module... done\n");

    // instantiating the commands
    CreatePathCommands();

    // addition objects
    PathGui::ViewProviderPath ::init();
    PathGui::ViewProviderPathCompound ::init();
    PathGui::ViewProviderPathCompoundPython ::init();
    PathGui::ViewProviderPathShape ::init();
    PathGui::ViewProviderPathPython ::init();
    PathGui::ViewProviderArea ::init();
    PathGui::ViewProviderAreaPython ::init();
    PathGui::ViewProviderAreaView ::init();
    PathGui::ViewProviderAreaViewPython ::init();

    // add resources and reloads the translators
    loadPathResource();

    // register preferences pages
    new Gui::PrefPageProducer<PathGui::DlgSettingsPathColor>(QT_TRANSLATE_NOOP("QObject", "CAM"));

    PyMOD_Return(mod);
}
