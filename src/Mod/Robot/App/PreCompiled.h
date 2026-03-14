#ifndef ROBOT_PRECOMPILED_H
#define ROBOT_PRECOMPILED_H

#include <FCConfig.h>

// STL
#include <memory>
#include <sstream>

// kdl_cp
#include "kdl_cp/chain.hpp"
#include "kdl_cp/chainfksolverpos_recursive.hpp"
#include "kdl_cp/chainiksolverpos_nr.hpp"
#include "kdl_cp/chainiksolverpos_nr_jl.hpp"
#include "kdl_cp/chainiksolvervel_pinv.hpp"
#include "kdl_cp/frames_io.hpp"
#include "kdl_cp/path_line.hpp"
#include "kdl_cp/path_roundedcomposite.hpp"
#include "kdl_cp/rotational_interpolation_sa.hpp"
#include "kdl_cp/trajectory_composite.hpp"
#include "kdl_cp/trajectory_segment.hpp"
#include "kdl_cp/utilities/error.h"
#include "kdl_cp/velocityprofile_trap.hpp"

// OCC
#include <BRepAdaptor_Curve.hxx>
#include <CPnts_AbscissaPoint.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#endif
