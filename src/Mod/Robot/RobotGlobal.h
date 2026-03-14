#ifndef ROBOT_GLOBAL_H
#define ROBOT_GLOBAL_H

#include <FCGlobal.h>

// Robot
#ifndef RobotExport
# ifdef Robot_EXPORTS
#  define RobotExport FREECAD_DECL_EXPORT
# else
#  define RobotExport FREECAD_DECL_IMPORT
# endif
#endif

// RobotGui
#ifndef RobotGuiExport
# ifdef RobotGui_EXPORTS
#  define RobotGuiExport FREECAD_DECL_EXPORT
# else
#  define RobotGuiExport FREECAD_DECL_IMPORT
# endif
#endif

#endif  // ROBOT_GLOBAL_H
