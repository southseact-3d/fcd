#ifndef MESH_PART_GUI_PRECOMPILED_H
#define MESH_PART_GUI_PRECOMPILED_H

#include <FCConfig.h>

// STL
#include <sstream>

// Qt Toolkit
#include <QApplication>
#include <QFuture>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QPointer>
#include <QPushButton>
#include <QStatusBar>
#include <QTimer>
#include <QtConcurrentMap>

// OCCT
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Poly_Polygon3D.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pnt.hxx>

// Inventor
#include <Inventor/SoPickedPoint.h>
#include <Inventor/details/SoFaceDetail.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoSeparator.h>

#endif  // __PRECOMPILED_GUI__
