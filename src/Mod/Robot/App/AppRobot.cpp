#include <Base/Console.h>
#include <Base/Interpreter.h>

#include "Edge2TracObject.h"
#include "PropertyTrajectory.h"
#include "Robot6Axis.h"
#include "Robot6AxisPy.h"
#include "RobotObject.h"
#include "Simulation.h"
#include "Trajectory.h"
#include "TrajectoryCompound.h"
#include "TrajectoryDressUpObject.h"
#include "TrajectoryObject.h"
#include "TrajectoryPy.h"
#include "Waypoint.h"
#include "WaypointPy.h"


namespace Robot
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("Robot")
    {
        add_varargs_method(
            "simulateToFile",
            &Module::simulateToFile,
            "simulateToFile(Robot,Trajectory,TickSize,FileName) - runs the "
            "simulation and write the result to a file."
        );
        initialize("This module is the Robot module.");  // register with Python
    }

private:
    Py::Object simulateToFile(const Py::Tuple& args)
    {
        PyObject* pcRobObj;
        PyObject* pcTracObj;
        float tick;
        char* FileName;

        if (!PyArg_ParseTuple(
                args.ptr(),
                "O!O!fs",
                &(Robot6AxisPy::Type),
                &pcRobObj,
                &(TrajectoryPy::Type),
                &pcTracObj,
                &tick,
                &FileName
            )) {
            throw Py::Exception();
        }

        try {
            Robot::Trajectory& Trac = *static_cast<TrajectoryPy*>(pcTracObj)->getTrajectoryPtr();
            Robot::Robot6Axis& Rob = *static_cast<Robot6AxisPy*>(pcRobObj)->getRobot6AxisPtr();
            Simulation Sim(Trac, Rob);
        }
        catch (const Base::Exception& e) {
            throw Py::RuntimeError(e.what());
        }

        return Py::Float(0.0);
    }
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace Robot


/* Python entry */
PyMOD_INIT_FUNC(Robot)
{
    // clang-format off
    // load dependent module
    try {
        Base::Interpreter().runString("import Part");
    }
    catch(const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }

    PyObject* robotModule = Robot::initModule();
    Base::Console().log("Loading Robot module… done\n");

    // Add Types to module
    Base::Interpreter().addType(&Robot::Robot6AxisPy          ::Type,robotModule,"Robot6Axis");
    Base::Interpreter().addType(&Robot::WaypointPy            ::Type,robotModule,"Waypoint");
    Base::Interpreter().addType(&Robot::TrajectoryPy          ::Type,robotModule,"Trajectory");


    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.

    Robot::Robot6Axis              ::init();
    Robot::RobotObject             ::init();
    Robot::TrajectoryObject        ::init();
    Robot::Edge2TracObject         ::init();
    Robot::Waypoint                ::init();
    Robot::Trajectory              ::init();
    Robot::PropertyTrajectory      ::init();
    Robot::TrajectoryCompound      ::init();
    Robot::TrajectoryDressUpObject ::init();

    PyMOD_Return(robotModule);
    // clang-format on
}
