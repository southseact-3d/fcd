#ifndef MEASUREGUI_PRECOMPILED_H
#define MEASUREGUI_PRECOMPILED_H

#include <FCConfig.h>

#include <Mod/Measure/MeasureGlobal.h>

#ifdef FC_OS_WIN32
# include <windows.h>
#endif

// standard
#include <cmath>

// STL
#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// OpenCasCade
#include <Mod/Part/App/OpenCascadeAll.h>

// Boost
#include <boost/regex.hpp>
#include <boost/algorithm/string/predicate.hpp>

// GL
// Include glext before QtAll/InventorAll
#ifdef FC_OS_WIN32
# include <GL/gl.h>
# include <GL/glext.h>
#else
# ifdef FC_OS_MACOSX
#  include <OpenGL/gl.h>
#  include <OpenGL/glext.h>
# else
#  ifndef GL_GLEXT_PROTOTYPES
#   define GL_GLEXT_PROTOTYPES 1
#  endif
#  include <GL/gl.h>
#  include <GL/glext.h>
# endif  // FC_OS_MACOSX
#endif   // FC_OS_WIN32
// Should come after glext.h to avoid warnings
#include <Inventor/C/glue/gl.h>

// Qt Toolkit
#include <Gui/QtAll.h>

// Inventor includes OpenGL
#include <Gui/InventorAll.h>

#endif
