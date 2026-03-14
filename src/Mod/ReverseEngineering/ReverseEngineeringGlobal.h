#include <FCGlobal.h>

#ifndef REEN_GLOBAL_H
# define REEN_GLOBAL_H


// Reen
# ifndef ReenExport
#  ifdef ReverseEngineering_EXPORTS
#   define ReenExport FREECAD_DECL_EXPORT
#  else
#   define ReenExport FREECAD_DECL_IMPORT
#  endif
# endif

// ReenGui
# ifndef ReenGuiExport
#  ifdef ReverseEngineeringGui_EXPORTS
#   define ReenGuiExport FREECAD_DECL_EXPORT
#  else
#   define ReenGuiExport FREECAD_DECL_IMPORT
#  endif
# endif

#endif  // REEN_GLOBAL_H
