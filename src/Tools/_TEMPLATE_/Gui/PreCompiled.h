#ifndef TEMPLATE_GUI_PRECOMPILED_H
#define TEMPLATE_GUI_PRECOMPILED_H

#include <FCConfig.h>

// Importing of App classes
#ifdef FC_OS_WIN32
# define _TEMPLATE_AppExport __declspec(dllimport)
# define _TEMPLATE_GuiExport __declspec(dllexport)
#else  // for Linux
# define _TEMPLATE_AppExport
# define _TEMPLATE_GuiExport
#endif

// standard
#include <cassert>
#include <cstdio>

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

#ifdef FC_OS_WIN32
# include <windows.h>
#endif

// Qt Toolkit
#include <Gui/QtAll.h>

#endif  // GUI_PRECOMPILED_H
