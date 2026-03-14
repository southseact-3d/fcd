#include "kdl_cp/chainfksolverpos_recursive.hpp"
#include "kdl_cp/chainiksolverpos_nr.hpp"
#include "kdl_cp/chainiksolvervel_pinv.hpp"
#include "kdl_cp/frames_io.hpp"


#include "RobotAlgos.h"


using namespace Robot;
using namespace std;
using namespace KDL;

//===========================================================================
// FeatureView
//===========================================================================

RobotAlgos::RobotAlgos() = default;

RobotAlgos::~RobotAlgos() = default;

void RobotAlgos::Test()
{
    // Definition of a kinematic chain & add segments to the chain
    KDL::Chain chain;
    chain.addSegment(Segment(Joint(Joint::RotZ), Frame(Vector(0.0, 0.0, 1.020))));
    chain.addSegment(Segment(Joint(Joint::RotX), Frame(Vector(0.0, 0.0, 0.480))));
    chain.addSegment(Segment(Joint(Joint::RotX), Frame(Vector(0.0, 0.0, 0.645))));
    chain.addSegment(Segment(Joint(Joint::RotZ)));
    chain.addSegment(Segment(Joint(Joint::RotX), Frame(Vector(0.0, 0.0, 0.120))));
    chain.addSegment(Segment(Joint(Joint::RotZ)));

    // Create solver based on kinematic chain
    ChainFkSolverPos_recursive fksolver = ChainFkSolverPos_recursive(chain);

    // Create joint array
    unsigned int nj = chain.getNrOfJoints();
    KDL::JntArray jointpositions = JntArray(nj);

    // Assign some values to the joint positions
    for (unsigned int i = 0; i < nj; i++) {
        float myinput;
        printf("Enter the position of joint %i: ", i);
        int result = scanf("%e", &myinput);
        (void)result;
        jointpositions(i) = (double)myinput;
    }

    // Create the frame that will contain the results
    KDL::Frame cartpos;

    // Calculate forward position kinematics
    int kinematics_status;
    kinematics_status = fksolver.JntToCart(jointpositions, cartpos);
    if (kinematics_status >= 0) {
        std::cout << cartpos << std::endl;
        printf("%s \n", "Success, thanks KDL!");
    }
    else {
        printf("%s \n", "Error: could not calculate forward kinematics :(");
    }

    // Creation of the solvers:
    ChainFkSolverPos_recursive fksolver1(chain);  // Forward position solver
    ChainIkSolverVel_pinv iksolver1v(chain);      // Inverse velocity solver
    ChainIkSolverPos_NR iksolver1(
        chain,
        fksolver1,
        iksolver1v,
        100,
        1e-6
    );  // Maximum 100 iterations, stop at accuracy 1e-6

    // Creation of jntarrays:
    JntArray result(chain.getNrOfJoints());
    JntArray q_init(chain.getNrOfJoints());

    // Set destination frame
    Frame F_dest = cartpos;

    iksolver1.CartToJnt(q_init, F_dest, result);

    for (unsigned int i = 0; i < nj; i++) {
        printf("Axle %i: %f \n", i, result(i));
    }
}

namespace Robot
{

KDL::Frame toFrame(const Base::Placement& To)
{
    return {
        KDL::Rotation::Quaternion(
            To.getRotation()[0],
            To.getRotation()[1],
            To.getRotation()[2],
            To.getRotation()[3]
        ),
        KDL::Vector(To.getPosition()[0], To.getPosition()[1], To.getPosition()[2])
    };
}

Base::Placement toPlacement(const KDL::Frame& frame)
{
    double x;
    double y;
    double z;
    double w;
    frame.M.GetQuaternion(x, y, z, w);
    return {Base::Vector3d(frame.p[0], frame.p[1], frame.p[2]), Base::Rotation(x, y, z, w)};
}

}  // namespace Robot
