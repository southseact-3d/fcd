#ifndef PATHSIMULATOR_PathSim_H
#define PATHSIMULATOR_PathSim_H

#include <memory>
#include <TopoDS_Shape.hxx>

#include <Mod/CAM/App/Command.h>
#include <Mod/Part/App/TopoShape.h>
#include <Mod/CAM/PathGlobal.h>

#include "VolSim.h"


using namespace Path;

namespace PathSimulator
{

/** The representation of a CNC Toolpath Simulator */

class PathSimulatorExport PathSim: public Base::BaseClass
{
    TYPESYSTEM_HEADER();

public:
    PathSim();
    ~PathSim();

    void BeginSimulation(Part::TopoShape* stock, float resolution);
    void SetToolShape(const TopoDS_Shape& toolShape, float resolution);
    Base::Placement* ApplyCommand(Base::Placement* pos, Command* cmd);

public:
    std::unique_ptr<cStock> m_stock;
    std::unique_ptr<cSimTool> m_tool;
};

}  // namespace PathSimulator


#endif  // PATHSIMULATOR_PathSim_H
