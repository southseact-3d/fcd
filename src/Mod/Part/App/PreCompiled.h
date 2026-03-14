#ifndef PART_PRECOMPILED_H
#define PART_PRECOMPILED_H

#include <FCConfig.h>

#include <Mod/Part/PartGlobal.h>

// standard
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <limits>

// STL
#include <array>
#include <fcntl.h>
#include <fstream>
#include <list>
#include <iostream>
#include <map>
#include <memory>
#include <numbers>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Qt
#include <QtGlobal>

// Boost
#include <boost/regex.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/random.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

// OpenCasCade
#include "OpenCascadeAll.h"

#if defined(FC_OS_WIN32)
# include <Windows.h>
#endif

#ifndef _Standard_Version_HeaderFile
# include <Standard_Version.hxx>
#endif

#endif
