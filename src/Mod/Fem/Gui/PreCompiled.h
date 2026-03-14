#ifndef FEMGUI_PRECOMPILED_H
#define FEMGUI_PRECOMPILED_H

#include <FCConfig.h>

// standard
#include <cassert>
#include <cmath>
#include <iostream>

// STL
#include <algorithm>
#include <bitset>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

// boost
#include <boost/algorithm/string.hpp>
#include <boost/bind/bind.hpp>
#include <boost/lexical_cast.hpp>

#ifdef FC_OS_WIN32
# include <windows.h>
#endif

// OCC
#include <BRepAdaptor_Surface.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

#include <QAction>
#include <QApplication>
#include <QDockWidget>
#include <QFile>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QMetaMethod>
#include <QPushButton>
#include <QSlider>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QTextCharFormat>
#include <QTextStream>
#include <QThread>
#include <QToolTip>
#include <qobject.h>

// inventor
#include <Inventor/SbVec3f.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/details/SoFaceDetail.h>
#include <Inventor/details/SoLineDetail.h>
#include <Inventor/details/SoPointDetail.h>
#include <Inventor/draggers/SoCenterballDragger.h>
#include <Inventor/draggers/SoHandleBoxDragger.h>
#include <Inventor/draggers/SoJackDragger.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/draggers/SoTransformerDragger.h>
#include <Inventor/engines/SoDecomposeMatrix.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/manips/SoCenterballManip.h>
#include <Inventor/manips/SoHandleBoxManip.h>
#include <Inventor/manips/SoJackManip.h>
#include <Inventor/manips/SoTabBoxManip.h>
#include <Inventor/manips/SoTransformBoxManip.h>
#include <Inventor/manips/SoTransformManip.h>
#include <Inventor/manips/SoTransformerManip.h>
#include <Inventor/nodes/SoAnnotation.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoDepthBuffer.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoFontStyle.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoIndexedPointSet.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoMarkerSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/nodes/SoMultipleCopy.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoPolygonOffset.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoSurroundScale.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransparencyType.h>


// Salomesh
#include <SMDSAbs_ElementType.hxx>
#include <SMESHDS_Mesh.hxx>
#include <SMESH_Mesh.hxx>
#include <SMESH_MeshEditor.hxx>

// VTK
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>

#endif  // FEMGUI_PRECOMPILED_H
