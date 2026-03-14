#include <string>
#include <vector>


#include "CAMSim.h"
#include "DlgCAMSimulator.h"
#include <Mod/Part/App/BRepMesh.h>


using namespace Base;
using namespace CAMSimulator;

TYPESYSTEM_SOURCE(CAMSimulator::CAMSim, Base::BaseClass);

void CAMSim::BeginSimulation(const Part::TopoShape& stock, float quality)
{
    DlgCAMSimulator::instance()->startSimulation(stock, quality);
}

void CAMSimulator::CAMSim::resetSimulation()
{
    DlgCAMSimulator::instance()->resetSimulation();
}

void CAMSim::addTool(
    const std::vector<float>& toolProfilePoints,
    int toolNumber,
    float diameter,
    float resolution
)
{
    DlgCAMSimulator::instance()->addTool(toolProfilePoints, toolNumber, diameter, resolution);
}

void CAMSimulator::CAMSim::SetBaseShape(const Part::TopoShape& baseShape, float resolution)
{
    if (baseShape.isNull()) {
        return;
    }

    DlgCAMSimulator::instance()->setBaseShape(baseShape, resolution);
}

void CAMSim::AddCommand(Command* cmd)
{
    std::string gline = cmd->toGCode();
    DlgCAMSimulator::instance()->addGcodeCommand(gline.c_str());
}
