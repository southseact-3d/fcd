#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/Parameter.h>

#include "Blending/BlendCurvePy.h"
#include "Blending/BlendPointPy.h"
#include "Blending/FeatureBlendCurve.h"

#include "FeatureCut.h"
#include "FeatureExtend.h"
#include "FeatureFilling.h"
#include "FeatureGeomFillSurface.h"
#include "FeatureSections.h"
#include "FeatureSewing.h"

#include "Measure.h"


namespace Surface
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("Surface")
    {
        initialize("This module is the Surface module.");  // register with Python
    }

private:
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace Surface

/* Python entry */
PyMOD_INIT_FUNC(Surface)
{
    try {
        Base::Interpreter().runString("import Part");
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }

    PyObject* mod = Surface::initModule();
    Base::Console().log("Loading Surface module… done\n");
    Base::Interpreter().addType(&Surface::BlendPointPy::Type, mod, "BlendPoint");
    Base::Interpreter().addType(&Surface::BlendCurvePy::Type, mod, "BlendCurve");

    // clang-format off
    // Add types to module
    Surface::Filling           ::init();
    Surface::Sewing            ::init();
    Surface::Cut               ::init();
    Surface::GeomFillSurface   ::init();
    Surface::Extend            ::init();
    Surface::FeatureBlendCurve ::init();
    Surface::Sections          ::init();
    // clang-format on

    // connect to unified measurement facility
    Surface::Measure ::initialize();

    PyMOD_Return(mod);
}
