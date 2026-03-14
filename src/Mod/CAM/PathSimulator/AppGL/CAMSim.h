#ifndef CAMSimulator_CAMSim_H
#define CAMSimulator_CAMSim_H

#include <memory>
#include <TopoDS_Shape.hxx>

#include <Mod/CAM/App/Command.h>
#include <Mod/CAM/PathGlobal.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Part/App/TopoShape.h>

#include "DlgCAMSimulator.h"

using namespace Path;

namespace CAMSimulator
{

/** The representation of a CNC Toolpath Simulator */

class CAMSimulatorExport CAMSim: public Base::BaseClass
{
    // TYPESYSTEM_HEADER();

public:
    static Base::Type getClassTypeId();
    Base::Type getTypeId() const override;
    static void init();
    static void* create();

private:
    static Base::Type classTypeId;


public:
    CAMSim() = default;

    void BeginSimulation(const Part::TopoShape& stock, float resolution);
    void resetSimulation();
    void addTool(
        const std::vector<float>& toolProfilePoints,
        int toolNumber,
        float diameter,
        float resolution
    );
    void SetBaseShape(const Part::TopoShape& baseShape, float resolution);
    void AddCommand(Command* cmd);
};

}  // namespace CAMSimulator


#endif  // CAMSimulator_CAMSim_H
