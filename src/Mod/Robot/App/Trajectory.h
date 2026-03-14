#ifndef ROBOT_Trajectory_H
#define ROBOT_Trajectory_H

#include <vector>

#include <Base/Persistence.h>
#include <Base/Placement.h>

#include <Mod/Robot/RobotGlobal.h>

#include "Waypoint.h"


namespace KDL
{
class Trajectory_Composite;
}

namespace Robot
{


/** The representation of a Trajectory
 */
class RobotExport Trajectory: public Base::Persistence
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    Trajectory();
    Trajectory(const Trajectory&);
    ~Trajectory() override;

    Trajectory& operator=(const Trajectory&);

    // from base class
    unsigned int getMemSize() const override;
    void Save(Base::Writer& /*writer*/) const override;
    void Restore(Base::XMLReader& /*reader*/) override;

    // interface
    void generateTrajectory();
    void addWaypoint(const Waypoint& WPnt);
    unsigned int getSize() const
    {
        return vpcWaypoints.size();
    }
    const Waypoint& getWaypoint(unsigned int pos) const
    {
        return *vpcWaypoints[pos];
    }
    std::string getUniqueWaypointName(const char* Name) const;
    const std::vector<Waypoint*>& getWaypoints() const
    {
        return vpcWaypoints;
    }

    /// delete the last n waypoints
    void deleteLast(unsigned int n = 1);
    /// return the Length (mm) of the Trajectory if -1 or of the Waypoint with the given number
    double getLength(int n = -1) const;
    /// return the duration (s) of the Trajectory if -1 or of the Waypoint with the given number
    double getDuration(int n = -1) const;
    Base::Placement getPosition(double time) const;
    double getVelocity(double time) const;


protected:
    std::vector<Waypoint*> vpcWaypoints;

    KDL::Trajectory_Composite* pcTrajectory {nullptr};
};

}  // namespace Robot


#endif  // PART_TOPOSHAPE_H
