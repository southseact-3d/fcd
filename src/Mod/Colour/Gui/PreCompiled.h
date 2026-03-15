// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_GUI_PRECOMPILED_H
#define COLOUR_GUI_PRECOMPILED_H

#include <FCConfig.h>

#ifdef FC_OS_WIN32
# include <windows.h>
#endif

// OCC
#include <Standard_Version.hxx>
#include <BRep_Tool.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <Poly_Triangulation.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

// stl
#include <limits>
#include <map>
#include <vector>

// Qt
#include <Gui/QtAll.h>

// Inventor
#include <Gui/InventorAll.h>

#endif  // COLOUR_GUI_PRECOMPILED_H
