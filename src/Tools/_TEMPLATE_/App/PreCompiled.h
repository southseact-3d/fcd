#ifndef TEMPLATE_APP_PRECOMPILED_H
#define TEMPLATE_APP_PRECOMPILED_H

#include <FCConfig.h>

// Exporting of App classes
#ifdef FC_OS_WIN32
# define _TEMPLATE_AppExport __declspec(dllexport)
#else  // for Linux
# define _TEMPLATE_AppExport
#endif

// standard
#include <cassert>
#include <cstdio>
#include <iostream>

// STL
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

// Xerces
#include <xercesc/util/XercesDefs.hpp>

#endif
