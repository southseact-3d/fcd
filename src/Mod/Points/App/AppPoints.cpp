#include <Base/Console.h>
#include <Base/Interpreter.h>

#include "Points.h"
#include "PointsPy.h"
#include "Properties.h"
#include "PropertyPointKernel.h"
#include "Structured.h"


namespace Points
{
extern PyObject* initModule();
}

/* Python entry */
PyMOD_INIT_FUNC(Points)
{
    // clang-format off
    PyObject* pointsModule = Points::initModule();
    Base::Console().log("Loading Points module… done\n");

    // add python types
    Base::Interpreter().addType(&Points::PointsPy::Type, pointsModule, "Points");

    // add properties
    Points::PropertyGreyValue       ::init();
    Points::PropertyGreyValueList   ::init();
    Points::PropertyNormalList      ::init();
    Points::PropertyCurvatureList   ::init();
    Points::PropertyPointKernel     ::init();

    // add data types
    Points::Feature                 ::init();
    Points::Structured              ::init();
    Points::FeatureCustom           ::init();
    Points::StructuredCustom        ::init();
    Points::FeaturePython           ::init();
    PyMOD_Return(pointsModule);
    // clang-format on
}
