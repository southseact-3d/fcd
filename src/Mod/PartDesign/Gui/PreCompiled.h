#ifndef PART_DESIGN_PRECOMPILED_H
#define PART_DESIGN_PRECOMPILED_H

#include <FCConfig.h>

#ifdef FC_OS_WIN32
# include <windows.h>
#endif

// Boost
#include <boost/core/ignore_unused.hpp>

// OCC
#include <Standard_Version.hxx>
#include <Bnd_Box.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRep_Tool.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

// stl
#include <limits>

// Qt
#include <Gui/QtAll.h>

// Inventor
#include <Gui/InventorAll.h>

#endif  // PART_DESIGN_PRECOMPILED_H
