#ifndef IMPORT_GUI_PRECOMPILED_H
#define IMPORT_GUI_PRECOMPILED_H

#include <FCConfig.h>

// standard
#include <cassert>
#include <iostream>
#include <list>

// STL
#include <map>
#include <set>
#include <string>
#include <vector>

#ifdef FC_OS_WIN32
# include <windows.h>
#endif

// OpenCasCade Base
#include <Mod/Part/App/OpenCascadeAll.h>

// OpenCascade View
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_SphericalSurface.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_TagSource.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Trsf.hxx>

// Qt Toolkit
#include <Gui/QtAll.h>

#endif  // __PRECOMPILED_GUI__
