#ifndef _RobotAlgos_h_
#define _RobotAlgos_h_

#include "kdl_cp/frames_io.hpp"
#include <Base/Placement.h>
#include <Base/Vector3D.h>
#include <Mod/Robot/RobotGlobal.h>

namespace Robot
{

/** Algo class for projecting shapes and creating SVG output of it
 */
class RobotExport RobotAlgos
{

public:
    /// Constructor
    RobotAlgos();
    virtual ~RobotAlgos();

    void Test();
};

KDL::Frame toFrame(const Base::Placement& To);

Base::Placement toPlacement(const KDL::Frame& frame);

}  // namespace Robot

#endif
