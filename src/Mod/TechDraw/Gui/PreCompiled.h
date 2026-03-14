#ifndef DRAWINGGUI_PRECOMPILED_H
#define DRAWINGGUI_PRECOMPILED_H

#include <FCConfig.h>

#ifdef FC_OS_WIN32
#endif

// standard
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>

// STL
#include <algorithm>
#include <regex>
#include <string>
#include <vector>

#ifdef FC_OS_WIN32
# include <windows.h>
# undef small
#endif

// Qt Toolkit
#ifndef _QtAll__
# include <Gui/QtAll.h>
#endif

// OpenCasCade
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepLProp_SLProps.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <TopExp.hxx>
#include <TopoDS_Shape.hxx>

// Open Inventor
#include <Inventor/SbVec3f.h>

#endif // DRAWINGGUI_PRECOMPILED_H
