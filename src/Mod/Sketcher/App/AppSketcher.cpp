#include <Base/Console.h>
#include <Base/Interpreter.h>

#include "Constraint.h"
#include "ConstraintPy.h"
#include "ExternalGeometryExtension.h"
#include "ExternalGeometryExtensionPy.h"
#include "ExternalGeometryFacade.h"
#include "ExternalGeometryFacadePy.h"
#include "GeometryFacade.h"
#include "GeometryFacadePy.h"
#include "PropertyConstraintList.h"
#include "Sketch.h"
#include "SketchGeometryExtension.h"
#include "SketchGeometryExtensionPy.h"
#include "SketchObject.h"
#include "SketchObjectSF.h"
#include "SketchPy.h"
#include "SolverGeometryExtension.h"
#include "Measure.h"

namespace Sketcher
{
extern PyObject* initModule();
}

/* Python entry */
PyMOD_INIT_FUNC(Sketcher)
{
    // load dependent module
    try {
        Base::Interpreter().runString("import Part");
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }

    PyObject* sketcherModule = Sketcher::initModule();

    // Add Types to module
    Base::Interpreter().addType(&Sketcher::ConstraintPy ::Type, sketcherModule, "Constraint");
    Base::Interpreter().addType(&Sketcher::SketchPy ::Type, sketcherModule, "Sketch");
    Base::Interpreter().addType(
        &Sketcher::ExternalGeometryExtensionPy ::Type,
        sketcherModule,
        "ExternalGeometryExtension"
    );
    Base::Interpreter().addType(
        &Sketcher::SketchGeometryExtensionPy ::Type,
        sketcherModule,
        "SketchGeometryExtension"
    );
    Base::Interpreter().addType(&Sketcher::GeometryFacadePy ::Type, sketcherModule, "GeometryFacade");
    Base::Interpreter().addType(
        &Sketcher::ExternalGeometryFacadePy ::Type,
        sketcherModule,
        "ExternalGeometryFacade"
    );


    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.

    Sketcher::SketchGeometryExtension ::init();
    Sketcher::ExternalGeometryExtension ::init();
    Sketcher::SolverGeometryExtension ::init();
    Sketcher::GeometryFacade ::init();
    Sketcher::ExternalGeometryFacade ::init();
    Sketcher::SketchObjectSF ::init();
    Sketcher::SketchObject ::init();
    Sketcher::SketchObjectPython ::init();
    Sketcher::Sketch ::init();
    Sketcher::Constraint ::init();
    Sketcher::PropertyConstraintList ::init();

    // connect to unified measurement facility
    Sketcher::Measure ::initialize();


    Base::Console().log("Loading Sketcher module… done\n");

    PyMOD_Return(sketcherModule);
}
