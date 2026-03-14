#ifndef SKETCHER_GUI_PRECOMPILED_H
#define SKETCHER_GUI_PRECOMPILED_H

#include <FCConfig.h>

// standard
#include <cmath>
#include <cstdlib>

// STL
#include <algorithm>
#include <bitset>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <vector>

// Boost
#include <boost/core/ignore_unused.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#ifdef FC_OS_WIN32
# include <windows.h>
#endif

// OpenCasCade
#include <BRep_Tool.hxx>
#include <GC_MakeEllipse.hxx>
#include <Precision.hxx>
#include <Standard_Version.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>

// Qt
#include <Gui/QtAll.h>

#include <QWidgetAction>

// all of Inventor
#include <Gui/InventorAll.h>

#endif  // SKETCHER_GUI_PRECOMPILED_H
