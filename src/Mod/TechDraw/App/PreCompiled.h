#ifndef TECHDRAW_PRECOMPILED_H
#define TECHDRAW_PRECOMPILED_H

#include <FCConfig.h>

// standard
#include <algorithm>
#include <cstdio>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// boost
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/is_kuratowski_subgraph.hpp>
#include <boost/random.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost_regex.hpp>

// Qt
#include <QApplication>
#include <QCollator>
#include <QDateTime>
#include <QDomDocument>
#include <QFile>
#include <QLocale>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QtConcurrentRun>

// OpenCasCade
#include <Mod/Part/App/OpenCascadeAll.h>

#endif
