#include <Base/Console.h>
#include <Base/Interpreter.h>

#include "AreaPy.h"
#include "Command.h"
#include "CommandPy.h"
#include "FeatureArea.h"
#include "FeaturePath.h"
#include "FeaturePathCompound.h"
#include "FeaturePathShape.h"
#include "Path.h"
#include "PathPy.h"
#include "PropertyPath.h"
#include "Voronoi.h"
#include "VoronoiCell.h"
#include "VoronoiCellPy.h"
#include "VoronoiEdge.h"
#include "VoronoiEdgePy.h"
#include "VoronoiPy.h"
#include "VoronoiVertex.h"
#include "VoronoiVertexPy.h"


namespace PathApp
{
extern PyObject* initModule();
}

/* Python entry */
PyMOD_INIT_FUNC(PathApp)
{
    // load dependent module
    try {
        Base::Interpreter().runString("import Part");
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }

    PyObject* pathModule = PathApp::initModule();
    Base::Console().log("Loading Path module... done\n");

    Py::Object module(pathModule);

    // Add Types to module
    Base::Interpreter().addType(&Path::CommandPy ::Type, pathModule, "Command");
    Base::Interpreter().addType(&Path::PathPy ::Type, pathModule, "Path");
    Base::Interpreter().addType(&Path::AreaPy ::Type, pathModule, "Area");

    PyObject* voronoiModule(module.getAttr("Voronoi").ptr());
    Base::Interpreter().addType(&Path::VoronoiPy ::Type, voronoiModule, "Diagram");
    Base::Interpreter().addType(&Path::VoronoiCellPy ::Type, voronoiModule, "Cell");
    Base::Interpreter().addType(&Path::VoronoiEdgePy ::Type, voronoiModule, "Edge");
    Base::Interpreter().addType(&Path::VoronoiVertexPy ::Type, voronoiModule, "Vertex");

    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.
    Path::Command ::init();
    Path::Toolpath ::init();
    Path::PropertyPath ::init();
    Path::Feature ::init();
    Path::FeaturePython ::init();
    Path::FeatureCompound ::init();
    Path::FeatureCompoundPython ::init();
    Path::FeatureShape ::init();
    Path::FeatureShapePython ::init();
    Path::Area ::init();
    Path::FeatureArea ::init();
    Path::FeatureAreaPython ::init();
    Path::FeatureAreaView ::init();
    Path::FeatureAreaViewPython ::init();
    Path::Voronoi ::init();
    Path::VoronoiCell ::init();
    Path::VoronoiEdge ::init();
    Path::VoronoiVertex ::init();

    PyMOD_Return(pathModule);
}
