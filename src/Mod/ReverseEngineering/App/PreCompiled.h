#ifndef REVERSE_ENGINEERING_APP_PRECOMPILED_H
#define REVERSE_ENGINEERING_APP_PRECOMPILED_H

#include <FCConfig.h>

// pcl headers include <boost/bind.hpp> instead of <boost/bind/bind.hpp>
#ifndef BOOST_BIND_GLOBAL_PLACEHOLDERS
# define BOOST_BIND_GLOBAL_PLACEHOLDERS
#endif


// standard
#include <map>

// boost
#include <boost/math/special_functions/fpclassify.hpp>

// OpenCasCade
#include <Geom_BSplineSurface.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <math_Gauss.hxx>
#include <math_Householder.hxx>

// Qt
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentMap>

#endif
