#include <FCGlobal.h>

#ifndef LAUNCHER_GLOBAL_H
# define LAUNCHER_GLOBAL_H


// Start
# ifndef StartExport
#  ifdef Start_EXPORTS
#   define StartExport FREECAD_DECL_EXPORT
#  else
#   define StartExport FREECAD_DECL_IMPORT
#  endif
# endif

// StartGui
# ifndef StartGuiExport
#  ifdef StartGui_EXPORTS
#   define StartGuiExport FREECAD_DECL_EXPORT
#  else
#   define StartGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // LAUNCHER_GLOBAL_H
