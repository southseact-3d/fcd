#include <FCGlobal.h>

#ifndef PATH_GLOBAL_H
# define PATH_GLOBAL_H


// Path
# ifndef PathExport
#  ifdef Path_EXPORTS
#   define PathExport FREECAD_DECL_EXPORT
#  else
#   define PathExport FREECAD_DECL_IMPORT
#  endif
# endif

// PathGui
# ifndef PathGuiExport
#  ifdef PathGui_EXPORTS
#   define PathGuiExport FREECAD_DECL_EXPORT
#  else
#   define PathGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

// PathSimulator
# ifndef PathSimulatorExport
#  ifdef PathSimulator_EXPORTS
#   define PathSimulatorExport FREECAD_DECL_EXPORT
#  else
#   define PathSimulatorExport FREECAD_DECL_IMPORT
#  endif
# endif

// CAMSimulator (new GL simulator)
# ifndef CAMSimulatorExport
#  ifdef CAMSimulator_EXPORTS
#   define CAMSimulatorExport FREECAD_DECL_EXPORT
#  else
#   define CAMSimulatorExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // PATH_GLOBAL_H
