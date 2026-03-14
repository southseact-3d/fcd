#ifndef APP_PRECOMPILED_H
#define APP_PRECOMPILED_H

#include <FCConfig.h>

// standard
#include <cassert>
#include <csignal>
#include <cstdio>
#include <ctime>

#ifdef FC_OS_WIN32
#include <crtdbg.h>
#include <direct.h>
#include <windows.h>
#endif

#if defined(FC_OS_LINUX) || defined(FC_OS_MACOSX) || defined(FC_OS_BSD)
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#endif

// Streams
#include <iostream>
#include <sstream>

// STL
#include <array>
#include <bitset>
#include <chrono>
#if defined(FC_OS_WIN32)
#include <codecvt>
#endif
#include <exception>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Boost
#include <boost_graph_adjacency_list.hpp>
#include <fastsignals/signal.h>

#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/utility.hpp>
#include <boost/bind/bind.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scope_exit.hpp>

#include <fmt/format.h>

// Qt -- only QtCore
#include <QDir>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QString>

#endif  // APP_PRECOMPILED_H
