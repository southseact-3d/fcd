#include <QApplication>

#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>
#include <Base/Tools.h>
#include <Gui/Application.h>
#include <Gui/Language/Translator.h>
#include <Gui/WidgetFactory.h>

#include "DlgPrefsMeasureAppearanceImp.h"
#include "QuickMeasurePy.h"
#include "ViewProviderMeasureAngle.h"
#include "ViewProviderMeasureDistance.h"
#include "ViewProviderMeasureBase.h"


// use a different name to CreateCommand()
void CreateMeasureCommands();


namespace MeasureGui
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("MeasureGui")
    {
        initialize("This module is the MeasureGui module.");  // register with Python
    }

    ~Module() override = default;

private:
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace MeasureGui

/* Python entry */
PyMOD_INIT_FUNC(MeasureGui)
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        PyMOD_Return(nullptr);
    }

    // load dependent module
    try {
        Base::Interpreter().loadModule("Measure");
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }

    PyObject* mod = MeasureGui::initModule();
    Base::Console().log("Loading GUI of Measure module… done\n");

    // instantiating the commands
    CreateMeasureCommands();

    // clang-format off
    MeasureGui::DimensionLinear::initClass();

    MeasureGui::ViewProviderMeasureGroup               ::init();
    MeasureGui::ViewProviderMeasureBase                ::init();
    MeasureGui::ViewProviderMeasure                    ::init();
    MeasureGui::ViewProviderMeasureAngle               ::init();
    MeasureGui::ViewProviderMeasureDistance            ::init();

    MeasureGui::ViewProviderMeasureArea                ::init();
    MeasureGui::ViewProviderMeasureLength              ::init();
    MeasureGui::ViewProviderMeasurePosition            ::init();
    MeasureGui::ViewProviderMeasureRadius              ::init();
    MeasureGui::ViewProviderMeasureDiameter            ::init();
    MeasureGui::ViewProviderMeasureCOM                 ::init();
    // clang-format on

    // register preferences pages
    new Gui::PrefPageProducer<MeasureGui::DlgPrefsMeasureAppearanceImp>(
        QT_TRANSLATE_NOOP("QObject", "Measure")
    );

    //    Q_INIT_RESOURCE(Measure);

    Base::Interpreter().addType(&MeasureGui::QuickMeasurePy::Type, mod, "QuickMeasure");

    // Create a QuickMeasure instance
    new MeasureGui::QuickMeasure(QApplication::instance());

    PyMOD_Return(mod);
}
