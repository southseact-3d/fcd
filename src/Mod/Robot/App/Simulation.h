#ifndef _Simulation_h_
#define _Simulation_h_

#include <Base/Placement.h>

#include <Mod/Robot/RobotGlobal.h>

#include "Robot6Axis.h"
#include "Trajectory.h"


namespace Robot
{

/** Algo class for projecting shapes and creating SVG output of it
 */
class RobotExport Simulation
{

public:
    /// Constructor
    Simulation(const Trajectory& Trac, Robot6Axis& Rob);
    virtual ~Simulation();

    double getLength()
    {
        return Trac.getLength();
    }
    double getDuration()
    {
        return Trac.getDuration();
    }

    Base::Placement getPosition()
    {
        return Trac.getPosition(Pos);
    }
    double getVelocity()
    {
        return Trac.getVelocity(Pos);
    }

    void step(double tick);
    void setToWaypoint(unsigned int n);
    void setToTime(float t);
    // apply the start axis angles and set to time 0. Restores the exact start position
    void reset();

    double Pos {0.0};
    double Axis[6] {};
    double startAxis[6] {};

    Trajectory Trac;
    Robot6Axis& Rob;
    Base::Placement Tool;
};


}  // namespace Robot


#endif
