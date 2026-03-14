#ifndef INSPECTION_APP_PRECOMPILED_H
#define INSPECTION_APP_PRECOMPILED_H

#include <FCConfig.h>

// STL
#include <numeric>

// OCC
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepGProp_Face.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <gp_Pnt.hxx>

// boost
#include <boost/core/ignore_unused.hpp>

// Qt
#include <QEventLoop>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentMap>

#endif
