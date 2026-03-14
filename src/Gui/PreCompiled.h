#ifndef GUI_PRECOMPILED_H
#define GUI_PRECOMPILED_H

#include <FCConfig.h>

// standard
#include <cstdio>
#include <cassert>
#include <fcntl.h>
#include <cctype>
#include <typeinfo>

#ifdef FC_OS_WIN32
# include <Windows.h>
# include <io.h>
# include <shellapi.h>
#endif

// streams
#include <iostream>
#include <iomanip>

// STL
#include <algorithm>
#include <atomic>
#include <bitset>
#include <limits>
#include <list>
#include <map>
#include <numbers>
#include <optional>
#include <queue>
#include <random>
#include <ranges>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <ranges>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Boost
#include <boost_graph_adjacency_list.hpp>
#include <fastsignals/signal.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/bind/bind.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/program_options.hpp>
#include <boost/utility.hpp>

// Xerces
#include <xercesc/util/TranscodingException.hpp>
#include <xercesc/util/XMLString.hpp>

// Qt/OpenGL
#include <QOpenGLFramebufferObjectFormat>

// Keep this order to avoid compiler warnings
#include "QtAll.h"
#include "InventorAll.h"

#if defined(FC_OS_WIN32)
# include <windows.h>
#endif

#endif  // GUI_PRECOMPILED_H
