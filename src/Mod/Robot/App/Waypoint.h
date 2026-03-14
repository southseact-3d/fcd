#ifndef ROBOT_WAYPOINT_H
#define ROBOT_WAYPOINT_H

#include <Base/Persistence.h>
#include <Base/Placement.h>
#include <Mod/Robot/RobotGlobal.h>


namespace Robot
{

/** The representation of a waypoint in a trajectory
 */
class RobotExport Waypoint: public Base::Persistence
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    enum WaypointType
    {
        UNDEF,
        PTP,
        LINE,
        CIRC,
        WAIT
    };

    Waypoint();
    /// full constructor
    Waypoint(
        const char* name,
        const Base::Placement& endPos,
        WaypointType type = Waypoint::LINE,
        float velocity = 2000.0,
        float acceleration = 100.0,
        bool cont = false,
        unsigned int tool = 0,
        unsigned int base = 0
    );

    ~Waypoint() override;

    // from base class
    unsigned int getMemSize() const override;
    void Save(Base::Writer& /*writer*/) const override;
    void Restore(Base::XMLReader& /*reader*/) override;


    std::string Name;
    WaypointType Type;
    float Velocity;
    float Acceleration;
    bool Cont;
    unsigned int Tool, Base;
    Base::Placement EndPos;
};

}  // namespace Robot


#endif  // ROBOT_WAYPOINT_H
